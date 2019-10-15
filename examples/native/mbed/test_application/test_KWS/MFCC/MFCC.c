#include "MFCC.h"

#ifdef STACK_SIZE
#undef STACK_SIZE
#endif
#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#include "FFT_Lib.h"
#include "MFCC_FB40.def"

#define Q15            15
#define Q14            14
#define Q12            12
#define Q6             6

#define MAXPOW         24
#define FFT2_SCALEDOWN 1
int pow_2[MAXPOW];
int pow_4[MAXPOW];

struct complex_f {
    float r;
    float i;
};

/*
  Radix 2, Decimated in Frequency, fft.
  Input are natural order, output is digitally-reversed.
  The last stage is handled differently since twidlles are (1, 0) leading to a some cycle count reduction
*/
/* Reorder from natural indexes to digitally-reversed one. Uses a pre computed LUT */
#define abs(a)  (((a)<0) ? (-(a)) : (a))

//#define QN 24
//#define log2  ((int) floor(log(2)*(float)(1<<QN)+0.5))
//#define c1_3   ((int) floor((1.0/3.0) * (float)(1<<QN) +0.5))
#define c1_3 (0xd6)
// normalisation factor in (32-QN).QN

//#define LOG_NORM  ((int) floor(log((float)N_FFT)*(float)(1<<QN)+0.5))
#define LOG_NORM    (0x400) 

int MFCC_Logfp(unsigned int a)
{
#ifdef PRINTDEB_FLOAT 
    float aa = (float)a/(float)(1<<QN);
#endif
    // comput log in base 2
    //if (a==0) a=0x1;
    int iLog2N  = __FL1(a);
    //printf("ln(2) = %x  1/3 = %x\n", log2, c1_3);

    int valint = iLog2N - QN;


    if (valint>=0) {
        if (a<(unsigned int)(1<<iLog2N)) valint++;
        a  = a >> valint;
    }
    else {
        a = a << (-valint);
    }
    //printf("a %x int %d\n",a,valint);

    int ar = (1<<QN) - a; // (1-ar = a):  log(1-ar) ~ -ar + ar*ar/2 - ar*ar*ar/3
    int ar2 = (ar * ar) >> (QN+1);
    int ar3 = (ar2 * ar) >> (QN-1);
    //printf("ar %x ar2 %x ar3 %x\n",ar,ar2,ar3);
    ar3 = (ar3 * c1_3) >> QN;
    int loga =  ((((valint)*GAPLOG2)) - ar - ar2 - ar3 ) ;
    //printf("loga %x\n", ((valint<<QN)*log2)>>QN);

#ifdef PRINTDEB_FLOAT
    printf("log(%f) %f %f (%x)\n",aa ,loga/(float)(1<<QN) ,log(aa),loga);
#endif

    return ( int) loga;
}


void MFCC_ComputeLog(unsigned int *__restrict__ MFCC) {
    int i;
    // log is natural log in C
    int TMP;

#ifdef PRINTDEB_FLOAT
    printf("log norm ((%f) = %x\n",LOG_NORM/(float)(1<<QN),LOG_NORM);
#endif

    for (i=0;i<MFCC_BANK_CNT;i++){
        TMP = MFCC_Logfp(MFCC[i])  ;
        //printf("log %x %x\n",MFCC[i],TMP);
        MFCC[i]=TMP-LOG_NORM;
    }
}



/* Reorder from natural indexes to digitally-reversed one. Uses a pre computed LUT */
void SwapSamples_args(        v2s *__restrict__ Data,
                              short int *__restrict__ SwapTable ,
                              int Ni)
{
    int i;

    for (i = 0; i < Ni; i++) {
        v2s S = Data[i];
        int SwapIndex = SwapTable[i];
        //printf("%d swapindex %d S %d %d\n",i,SwapIndex,S[0],S[1]);
        if (i < SwapIndex) {
            Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
        }
    }
}

void SwapSamples(SwapSamples_Arg_T *Arg)
{
    v2s *__restrict__ Data = Arg->Data;
    short *__restrict__ SwapTable = Arg->SwapTable;
    int Ni = Arg->Ni;
    int i;

    for (i = 0; i < Ni; i++) {
        v2s S = Data[i];
        int SwapIndex = SwapTable[i];
        if (i < SwapIndex) {
            Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
        }
    }
}

void Radix2FFT_DIF_argsff(short int *__restrict__ Data, struct complex_f *DataVf, short int *__restrict__ Twiddles, int N_FFT2)
{

    //int iLog2N  = (FL1(N_FFT2)-1);
    int iLog2N  = __FL1(N_FFT2);
    printf("iLog2N %d\n",iLog2N);
    unsigned int iCnt1, iCnt2, iCnt3,
        iQ,    iL,    iM,
        iA,    iB;
    v2s *CoeffV = (v2s *) Twiddles;
    v2s *DataV  = (v2s *) Data;

#ifdef FLOAT
    struct complex_f Wf,Tmpf;
#endif

    iL = 1;
    iM = N_FFT2 / 2;

    for (iCnt1 = 0; iCnt1 < (unsigned int)(iLog2N-1); iCnt1++) {
        int max=0;
        iQ = 0;
        printf("stage %d\n",iCnt1);
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
#ifdef FLOAT
            twiddle(&Wf,N_FFT2,iQ);
#endif
            v2s W = CoeffV[iQ];
            //W[0] = (short int) (Wf.r * (1<<15));
            //W[1]=  (short int) (Wf.i * (1<<15));
            //W[1] = -W[1];
            // printf("fpfpfpfp %d %f %f %f %f (%f %f)\n",iQ,Wf.r,Wf.i,W[0]/(float)(1<<15),W[1]/(float)(1<<15),DataV[iA][0]/(float)(1<<15),DataV[iA][1]/(float)(1<<15));
            iA = iCnt2;
            for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                v2s Tmp;
                iB = iA + iM;
                Tmp = DataV[iA] - DataV[iB];
#ifdef FLOAT
                Tmpf.r = DataVf[iA].r - DataVf[iB].r;
                Tmpf.i = DataVf[iA].i - DataVf[iB].i;
                DataVf[iA].r = (DataVf[iA].r + DataVf[iB].r);
                DataVf[iA].i = (DataVf[iA].i + DataVf[iB].i);
                DataVf[iB].r = Tmpf.r * Wf.r - Tmpf.i * Wf.i;
                DataVf[iB].i = Tmpf.r * Wf.i + Tmpf.i * Wf.r;
#endif

                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
#ifdef EXT
                DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
#else
                DataV[iB] = __CPLX_MULS(Tmp, W) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
#endif

#ifdef FLOAT
                float  deltaiA= fabs(DataV[iA][0]/(float)(1<<(14-iCnt1))-DataVf[iA].r) + fabs(DataV[iA][1]/(float)(1<<(14-iCnt1))-DataVf[iA].i);
                float  deltaiB= fabs(DataV[iB][0]/(float)(1<<(14-iCnt1))-DataVf[iB].r) + fabs(DataV[iB][1]/(float)(1<<(14-iCnt1))-DataVf[iB].i);
                //printf("deltaiA %f deltaiB %f\n",deltaiA,deltaiB);
                if ((deltaiA>=DataVf[iA].r*0.05) || (deltaiB>=DataVf[iB].r*0.05)) printf("********");
                printf("fpfpfpfp %d iA %d iB %d (%f %f | %f %f) (%f %f | %f %f)\n",iQ,iA,iB,W[0]/(float)(1<<15),W[1]/(float)(1<<15),Wf.r,Wf.i,DataV[iA][0]/(float)(1<<(14-iCnt1)),DataV[iA][1]/(float)(1<<(14-iCnt1)),DataVf[iA].r,DataVf[iA].i);
                printf("fpfpfpfp\t\t (%f %f | %f %f)\n",DataV[iB][0]/(float)(1<<(14-iCnt1)),DataV[iB][1]/(float)(1<<(14-iCnt1)),DataVf[iB].r,DataVf[iB].i);
#endif

                iA = iA + 2 * iM;
            }
            iQ += iL;
        }
        iL <<= 1;
        iM >>= 1;
    }
    iA = 0;
    /* Last Layer: W = (1, 0) */
    int max=0;
    for (iCnt3 = 0; iCnt3 < (unsigned int)(N_FFT2>>1); iCnt3++) {
        v2s Tmp;
        iB = iA + 1;
        Tmp = DataV[iA] - DataV[iB];
        DataV[iA] = (DataV[iA] + DataV[iB]);
        DataV[iB] = Tmp;
#ifdef FLOAT
        Tmpf.r = DataVf[iA].r - DataVf[iB].r;
        Tmpf.i = DataVf[iA].i - DataVf[iB].i;
        DataVf[iA].r = (DataVf[iA].r + DataVf[iB].r);
        DataVf[iA].i = (DataVf[iA].i + DataVf[iB].i);
        DataVf[iB] = Tmpf;
        printf("fpfpfpfp %d iA %d iB %d (%f %f | %f %f) (%f %f | %f %f)\n",iQ,iA,iB,DataV[iA][0]/(float)(1<<(15-iCnt1)),DataV[iA][1]/(float)(1<<(15-iCnt1)),DataVf[iA].r,DataVf[iA].i,DataV[iB][0]/(float)(1<<(15-iCnt1)),DataV[iB][1]/(float)(1<<(15-iCnt1)),DataVf[iB].r,DataVf[iB].i);
#endif
        iA = iA + 2;
    }
}

void Radix2FFT_DIF_args(short int *__restrict__ Data,short int *__restrict__ Twiddles, int N_FFT2)
{

    //int iLog2N  = (FL1(N_FFT2)-1);
    int iLog2N  = __FL1(N_FFT2);
#ifdef PRINTDEB
    printf("iLog2N %d\n",iLog2N);
#endif
    unsigned int iCnt1, iCnt2, iCnt3,
        iQ,    iL,    iM,
        iA,    iB;
    v2s *CoeffV = (v2s *) Twiddles;
    v2s *DataV  = (v2s *) Data;
    iL = 1;
    iM = N_FFT2 / 2;

    for (iCnt1 = 0; iCnt1 < (unsigned int)(iLog2N-1); iCnt1++) {
        int max=0;
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
            v2s W = CoeffV[iQ];
            //W[0] = (short int) (wf.r * (1<<15));
            //W[1]=  (short int) (wf.i * (1<<15));
            //W[1] = -W[1];
            // printf("fpfpfpfp %d %f %f %f %f (%f %f)\n",iQ,Wf.r,Wf.i,W[0]/(float)(1<<15),W[1]/(float)(1<<15),DataV[iA][0]/(float)(1<<15),DataV[iA][1]/(float)(1<<15));
            iA = iCnt2;
            for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                v2s Tmp;
                iB = iA + iM;
                Tmp = DataV[iA] - DataV[iB];

                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
#ifdef EXT
                DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
#else
                DataV[iB] = __CPLX_MULS(Tmp, W) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
#endif
                iA = iA + 2 * iM;
            }
            iQ += iL;
        }
        iL <<= 1;
        iM >>= 1;
    }
    iA = 0;
    /* Last Layer: W = (1, 0) */
    int max=0;
    for (iCnt3 = 0; iCnt3 < (unsigned int)(N_FFT2>>1); iCnt3++) {
        v2s Tmp;
        iB = iA + 1;
        Tmp = DataV[iA] - DataV[iB];
        DataV[iA] = (DataV[iA] + DataV[iB]);
        DataV[iB] = Tmp;
        iA = iA + 2;
    }
}

void Radix2FFT_DIF(FFT_Arg_T *Arg)
{
    signed short * __restrict__ Data = Arg->Data;
    signed short * __restrict__ Twiddles = Arg->Twiddles;
    unsigned int N_FFT2 = Arg->N_fft;
    int iLog2N  = __FL1(N_FFT2);
    unsigned int iCnt1, iCnt2, iCnt3,
        iQ,    iL,    iM,
        iA,    iB;
    v2s *CoeffV = (v2s *) Twiddles;
    v2s *DataV  = (v2s *) Data;

    iL = 1;
    iM = N_FFT2 / 2;

    for (iCnt1 = 0; iCnt1 < (unsigned int)(iLog2N-1); iCnt1++) {
        iQ = 0;
        for (iCnt2 = 0; iCnt2 < iM; iCnt2++) {
            v2s W = CoeffV[iQ];
            iA = iCnt2;
            for (iCnt3 = 0; iCnt3 < iL; iCnt3++) {
                v2s Tmp;
                iB = iA + iM;
                Tmp = DataV[iA] - DataV[iB];
                DataV[iA] = (DataV[iA] + DataV[iB]) >> (v2s) {FFT2_SCALEDOWN, FFT2_SCALEDOWN};
                DataV[iB] = __CPLX_MULS_DIV2(Tmp, W);
                iA = iA + 2 * iM;
            }
            iQ += iL;
        }
        iL <<= 1;
        iM >>= 1;
    }
    iA = 0;
    for (iCnt3 = 0; iCnt3 < (N_FFT2>>1); iCnt3++) {
        v2s Tmp;
        iB = iA + 1;
        Tmp = (DataV[iA] - DataV[iB]);
        DataV[iA] = (DataV[iA] + DataV[iB]);
        DataV[iB] = Tmp;
        iA = iA + 2;
    }
}

void MFCC_PreEmphasis(short int * __restrict__ Frame, short int * __restrict__ Out, int FrameSize, short int S, short int  shift)
{
    static int Active = 1;
    static int Gain = 2;
    unsigned int i;

    // Y[n]=X[n]−0.95⋅X[n−1]

    for (i=0; i<(unsigned int)FrameSize; i++) {
        if (Active) {
            Out[i] = (Frame[i]<<shift) - __MULSRN(FP2FIX(0.97, Q15), S, 15);
            S = (Frame[i]<<shift);
        } else {
            Out[i] = Frame[i]<<Gain;
        }

    }
    //return S;
}

void MFCC_WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame,
                   short int *__restrict__ Window, int FrameSize, int FFT_Dim)
{
    unsigned int i;

    for (i=0; i<(unsigned int)FrameSize; i++) {
        /* Frame and Window are Q15, we want OutFrame with precision FFT2_SAMPLE_DYN */
        //  printf("win %d %f\n",i,Window[i]/(float) (1<<15));
        OutFrame[i] = (v2s) {__MULSRN(Frame[i], Window[i], 15), 0};
    }
    for (i=FrameSize; i<(unsigned int)FFT_Dim;i++) OutFrame[i] = (v2s) {0, 0};
}

#define    INVSQRT2    __FP2FIX(0.707106781, FFT2_SAMPLE_DYN)

//static void EstimatePower(v2s *__restrict__ FrameIn, short int *__restrict__ Power, int N, int N_fft, int SHIFT_RIGHT)


void MFCC_EstimatePowerInt(v2s *__restrict__ FrameIn,  int N, int N_fft)
{
    unsigned int i;
    int* Power_int = ( int*) FrameIn;

    for (i=0; i<(unsigned int)N; i++) {
        int P = __DOTP2(FrameIn[i], FrameIn[i]);
        //printf("\tpower (%x %x)\n",FrameIn[i][0],FrameIn[i][1]);
        Power_int[i] = P; // __MULSRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
        //printf("xxpower %d %x (%x)\n",i,P,FrameIn[i]);
    }
}


void MFCC_Compute(short int *__restrict__ FramePower,  int *__restrict__ MFCC)
{
    unsigned int i, j, k;
    int FilterBanks = MFCC_BANK_CNT;

    /*
      FramePower is in Q 6.10
      MFCC_Coeffs is in MFCC_COEFF_DYN    10
      MFCC is int  24.8
    */

    for (i=0; i<(unsigned int)FilterBanks; i++) {
        int Coeff = 0;
        int Base = MFCC_FilterBank[i].Base;
        int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);

        for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
            Coeff = Coeff + MFCC_Coeffs[Base+k]*FramePower[2*j];
            //printf("\tcoeff %d %d %d c=%x p=%x\n",i,k,j,Coeff,FramePower[2*j]);
        }
        //printf("mfcc coeff %d %x %f (%d) norm %d\n",i,Coeff,Coeff/(float)(1<<8),k,MFCC_FilterBank[i].Norm);

        MFCC[i] = Coeff;
    }
}

void MFCC_ComputeMFCCInt(unsigned int *__restrict__ FramePower,  unsigned int *__restrict__ MFCC)
{
    unsigned int i, j, k;
    int FilterBanks = MFCC_BANK_CNT;

    /*
      FramePower is in Q 18.14
      MFCC_Coeffs is in MFCC_COEFF_DYN    6.10
      MFCC is int  8.24
    */

    for (i=0; i<(unsigned int)FilterBanks; i++) {
        unsigned int Coeff = 0;
        int Base = MFCC_FilterBank[i].Base;
        int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);
        int maxin=0;

        for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
            if (FramePower[j]>(unsigned int)maxin) maxin =FramePower[j];
        }

        short int shift ;
        short int shift0 = __FL1(maxin);
        // check multiply overflow condition Q18.14 * Q6.10 => Q8.24
        if ((shift0+10)>31) shift = shift0-21; else shift = 0;
#ifdef PRINTDEB
        printf("leading 1 %d shift %d max %x\n",shift0,shift,maxin);
#endif
        for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
            // 6.10 * 0.16
            //printf("-->norm %x fp %x ",MFCC_FilterBank[i].Norm,FramePower[j]);
            //printf("fp %x\n", FramePower[j] );
            Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>shift);

#ifdef PRINTDEB
            printf("\tcoeff %d %d %d c=%E p=%E coeff=%f prod=%x\n",i,k,j,Coeff/(float)(1<<24)/(float)(1<<9),FramePower[j]/(float)(1<<23),MFCC_Coeffs[Base+k]/(float)(1<<10),MFCC_Coeffs[Base+k]*(FramePower[j]>>shift));
#endif
        }


#ifdef PRINTDEB
        // check overflow
        short int shiftc = __FL1(Coeff);
#endif

        Coeff = Coeff << shift;

#ifdef PRINTDEB
        if ((31-shiftc) < shift) printf("***********");
        printf("mfcc coeff %d %x %f (%d) norm %d\n",i,Coeff,Coeff/(float)(1<<(24))/(float) (1<<9),k,MFCC_FilterBank[i].Norm);
#endif

        MFCC[i] = Coeff;
    }
}

#ifdef FLOAT
struct complex_f in_dct_f[2*NDCT];
#endif

void MFCC_ComputeDCT(v2s *in_dct, v2s *twiddct, v2s *twiddles, short int *SwapLUT) {
    unsigned int i,j;

#ifdef dctfloat
    for(j=0;j<N_FFT;j++) {
        in_dct_f[j].r = in_dct[j][0]/(float) (1<<10);
        in_dct_f[j].i = in_dct[j][1]/(float) (1<<10);
    }
    radix2(in_dct_f,2*NDCT,0);
    for(j=0;j<N_FFT;j++) {
        in_dct[j][0] = (short int) floor(in_dct_f[j].r*(float) (1<<4));

        in_dct[j][1] = (short int) floor(in_dct_f[j].i*(float) (1<<4));
    }
#endif

    Radix2FFT_DIF_args((short int*)in_dct,(short int*)twiddles, 2*NDCT);
    //bit_reverse_reorder_int((short int *) in_dct, 2*NDCT, (int) 2);
    SwapSamples_args(in_dct,SwapLUT,2*NDCT);

    for (i=0;i<NDCT;i++) {
        v2s out_dct_loc = __CPLX_MULS(in_dct[i],twiddct[i]);
        in_dct[i] = out_dct_loc;
    }
}


void SwapSamples_real(        v2s *__restrict__ Data,
                              short *__restrict__ SwapTable ,
                              int Ni)
{

    int i;

    for (i = 1; i < Ni; i+=2) {
        v2s S = Data[i];
        int SwapIndex = SwapTable[i];
        if (i < SwapIndex) {
            Data[i] = Data[SwapIndex]; Data[SwapIndex] = S;
        }
    }
    // mirror the second half and take conjugate
    for (i = Ni/2 + 1; i < Ni; i++) {
        Data[Ni-i] = (v2s){Data[i][0],-Data[i][1]};
    }
}


#undef abs
