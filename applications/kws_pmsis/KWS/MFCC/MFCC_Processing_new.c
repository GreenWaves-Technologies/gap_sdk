#include "MFCC_Processing.h"
#include "LUT.def"
#include "Gap8.h"


extern  fbank_type_t MFCC_FilterBank[];
extern  short int MFCC_Coeffs[];

#define Abs(a) (((a)<0)?(-a):(a))
#define Minu(a, b) 		(( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )
#define __Ncore() __builtin_pulp_CoreCount()

#define COREID 0

#include "gap_sai.h"
extern performance_t perf;
//#define PERFF

//GAP_L1_FC_DATA InSig[4*160];

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

	NCore = __Ncore();
	Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;

}

void MFCC_Processing(KerMFCC_fpT* Arg)
{
    int i, j, k;
    int FrameCount = 0;
    int FilterBanks = MFCC_BANK_CNT;
    short int lastsmp=0;
    int maxin;
    short int shift;

    short int *InSignal = Arg->InSignal;
    v2s * W_Frame = Arg->W_Frame;
    short int *Frame = Arg->Frame;
    short int *FEAT_LIST = Arg->FEAT_LIST;
    short int *pMFCC_coeffs = Arg->mfcc_coeffs;
    
#ifdef RUN_MFCC_ON_CLUST
    // compute chunks for each core
    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize(N_FRAME);
#else
    // running on FC => only one core
    unsigned int CoreId = 0;
    unsigned int ChunkCell = N_FRAME;
#endif
    
    unsigned int First = FRAME_STEP*CoreId*ChunkCell;
    unsigned int Last  = FRAME_STEP*Minu(CoreId*ChunkCell+ChunkCell, N_FRAME);
    FrameCount=CoreId*ChunkCell;


#ifdef PRINTINT
    if (CoreId==COREID)
      for (i=0; i<(unsigned int)FilterBanks; i++) {
	int Base = MFCC_FilterBank[i].Base;
	int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);
	for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
	  printf("=> i %d k %d j %d start %d count %d MFCC_Coeffs  %d\n",i,k,j,MFCC_FilterBank[i].Start,Count,pMFCC_coeffs[Base+k]);
	
	}
      }
#endif
    

    //    for (i=0; i<FRAME_STEP*N_FRAME; i+=FRAME_STEP) {
    for (i=First; i<(int)Last; i+=FRAME_STEP) {

      
#ifdef PRINTDEB
      if (CoreId==0) DumpShortInt("Input", &InSignal[i], FRAME, FrameCount, Q15);
#endif
#ifdef PRINTINT
      if (CoreId==COREID) {
	printf("CHUNKsize %d First %d Last %d\n",ChunkCell,First,Last);
	printf("dump input %d\n",i);
        for (j=0; j<FRAME; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId,j);
            printf("%x ", InSignal[i+j]);
            if (!((j+1)%10)) printf("\n");
        }
      }
#endif

        lastsmp = (i==0)?0 : InSignal[i-1];
        maxin=Abs(lastsmp);
        for (j=0;j<FRAME;j++) if (Abs(InSignal[i+j])>maxin) maxin=Abs(InSignal[i+j]);

        shift = __FL1(maxin);

        if (shift<=13) shift = (15-shift-2); else shift=0;

	
#ifdef PERFF
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
	//printf("Id %d %d (%d) frame %p  insig %p\n",CoreId,i,CoreId*FRAME,Frame+CoreId*FRAME,InSignal+i);
	MFCC_PreEmphasis(InSignal+i , Frame+CoreId*FRAME, FRAME, lastsmp, shift);

	
#ifdef PRINTDEB
        if (CoreId==0) DumpShortInt("Premphasis Output", Frame+CoreId*FRAME, FRAME, FrameCount, Q15+shift);
#endif

        MFCC_WindowedFrame(Frame+CoreId*FRAME, W_Frame+CoreId*(N_FFT+4), HammingLUT, FRAME, N_FFT);

#ifdef PRINTDEB
        DumpComplex("Windowed", W_Frame+CoreId*(N_FFT+4), N_FFT, FrameCount, 15+shift);
#endif

#ifdef PRINTINT
	if (CoreId==COREID) {
        printf("dump windowed %d\n",i);
        for (j=0; j<N_FFT; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId,j);
	  printf("%x %x, ", W_Frame[j+CoreId*(N_FFT+4)][0], W_Frame[j+CoreId*(N_FFT+4)][1]);
	  if (!((j+1)%10)) printf("\n");
        }
	}
#endif
#ifdef PERFF	
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 0 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
        Radix2FFT_DIF_args((short int*)(W_Frame+CoreId*(N_FFT+4)),  TwiddlesLUT, N_FFT);

#ifdef PERFF	
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 1 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif

        SwapSamples_args(W_Frame+CoreId*(N_FFT+4),SwapLUT,N_FFT);

	
#ifdef PRINTDEB
        if (CoreId==0) DumpComplex("FFT", W_Frame, N_FFT, FrameCount, 7+shift);
#endif

        maxin=0;
       	short int *W_FrameS=(short int*)(W_Frame+CoreId*(N_FFT+4));

        for (j=0;j<N_FFT;j++) if (Abs(W_FrameS[j])>maxin) maxin=Abs(W_FrameS[j]);

        short int shift1 = __FL1(maxin);


        // 9.7
        MFCC_EstimatePowerInt(W_Frame+CoreId*(N_FFT+4), HALF_FFT, N_FFT);

	unsigned int* FramePower = ( unsigned int *) (W_Frame+CoreId*(N_FFT+4));

#ifdef PRINTINT
	if (CoreId==COREID) {
        printf("dump power %d\n",i);
        for (j=0; j<N_FFT; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId,j);
	  printf(" %x ", FramePower[j]);
	  if (!((j+1)%10)) printf("\n");
        }
	}
#endif
	
#ifdef PRINTDEB
        //   Q18.14  * 1/512  => factor 1/(1<<19)
        DumpInt("Power", FramePower, HALF_FFT, FrameCount, 9 + 2*shift + 14);
#endif
        // compute energy
        int energy=0;
        for(j=0;j<HALF_FFT;j++) energy += FramePower[j];

#ifdef PRINTDEB_FLOAT
        printf("energy00 = %f %x log = %f\n",energy/(float)(1<<(14+9+2*shift)),energy,(log(energy/(float)(1<<(14+9+2*shift)))));
#endif

        // MFCC_Logfp(energy*(1<<(-2*shift + QN - 14 - 9)))
        energy = MFCC_Logfp(energy>>(2*shift)) + (QN-14-9)*GAPLOG2;


        unsigned int * MFCC_BASE = (unsigned int*) FramePower;
#if 0
	if (i==COREID*FRAME_STEP) {
	  printf("===================> DUMP MFCC\n");
	  MFCC_ComputeMFCCInt_print(FramePower, MFCC_BASE);
	}
	else
#endif
	  MFCC_ComputeMFCCInt(FramePower, MFCC_BASE, pMFCC_coeffs);


#ifdef PRINTINT
    if (CoreId==COREID)
      for (i=0; i<(unsigned int)FilterBanks; i++) {
	int Base = MFCC_FilterBank[i].Base;
	int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);
	for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
	  printf("=> i %d k %d j %d start %d count %d MFCC_Coeffs  %d\n",i,k,j,MFCC_FilterBank[i].Start,Count,pMFCC_coeffs[Base+k]);
	
	}
      }
#endif

	

#ifdef PRINTINT
	if (CoreId==COREID) {
        printf("dump MFCC %d\n",i);
        for (j=0; j<MFCC_BANK_CNT; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId, j);
            printf(" %x ",  MFCC_BASE[j]);
            if (!((j+1)%10)) printf("\n");
        }
	}
#endif
#ifdef PRINTDEB
        // coeff 6.10, data 18.14  * 1/512 => 26
        DumpInt_norm("Filtered power", MFCC_BASE, MFCC_BANK_CNT, FrameCount, 9+24+2*shift  );
#endif
#ifdef PERFF	
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 11 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
	
        // 8.24
        MFCC_ComputeLog(MFCC_BASE);

        for (j=0;j<MFCC_BANK_CNT;j++) {
            MFCC_BASE[j] -= 2*shift*GAPLOG2;
        }

#ifdef PERFF	
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 2 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
	
#ifdef PRINTDEB
        DumpInt("LOG Filtered Power", MFCC_BASE, MFCC_BANK_CNT, FrameCount, QN);
#endif
        maxin=0;
        for (j=0;j<MFCC_BANK_CNT;j++)   if (Abs((int)MFCC_BASE[j])>maxin) maxin=Abs((int)MFCC_BASE[j]);
        shift = __FL1(maxin);


        //!!!!!!!!!!!!!!tocheck
        int rnd=0;
        if (shift<=15) shift=0; else shift = shift-14;
        if (shift) rnd = 1<<(shift-1);

        v2s * in_dct = (v2s *) MFCC_BASE;

        for (j=0;j<MFCC_BANK_CNT;j++) {
            // convert int to v2s with Im=0
            // scale/clip int 8.24 MFCC to short int 6.10 (no saturation!) and pad imaginary with 0
            in_dct[j] = (v2s) {(short int) (((int)MFCC_BASE[j] + rnd)  >> shift) , 0};

        }

        // pad with 0
        memset(in_dct+MFCC_BANK_CNT,0,(2*NDCT-MFCC_BANK_CNT)*sizeof(v2s));

        // in_dct 6.10
        //for (j=0;j<MFCC_BANK_CNT;j++) in_dct[j][0]=(short int) MFCC_BASE[j];
        //float out_dct_f[NDCT];
        //dct(in_dct,64,out_dct_f,10);
        //for(j=0;j<64;j++) printf("%d %f\n",j,out_dct_f[j]/(2.0*sqrt(2)));
#ifdef PRINTINT
	if (CoreId==COREID) {
        printf("dump input dct %d\n",i);
        for (j=0; j<2*NDCT; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId, j);
            printf(" %x %x, ", in_dct[j][0],in_dct[j][1]);
            if (!((j+1)%10)) printf("\n");
        }
	}
#endif

        MFCC_ComputeDCT(in_dct, (v2s *)twidend,  (v2s *)TwiddlesLUT_128, SwapLUT_128);

        //for(j=0;j<64;j++) in_dct[j][0] = (short int) floor(out_dct_f[j] * (1<<6) + 0.5);


        // NORMDCT Q15
        in_dct[0][0] = (in_dct[0][0] * NORMDCT) >> 15;
        for(j=0;j<64;j++) in_dct[j][0] = (in_dct[j][0] * NORMDCT) >> 15;

#ifdef PRINTINT
	if (CoreId==COREID) {
        printf("dump output dct %d\n",i);
        for (j=0; j<2*NDCT; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId, j);
            printf(" %x %x, ", in_dct[j][0],in_dct[j][1]);
            if (!((j+1)%10)) printf("\n");
        }
	}
#endif
	
#ifdef PERFF
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 3 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
	
        int *LIFTED = (int*) in_dct;

        for(j=0;j<NUMCEP;j++) LIFTED[j] = (int) in_dct[j][0];

#ifdef PRINTDEB
        DumpInt("DCT", LIFTED, MFCC_BANK_CNT, FrameCount, 6+(14-shift));
        //for (j=1; j<NUMCEP; j++) printf("lift coeff[%d] = %f\n",j,lift_coeff[j]/(float) (1<<11));
#endif
        //lifting coeff Q5.11 ([-10..12] MFCC 6+(14-shift)
        for (j=0; j<NUMCEP; j++) LIFTED[j] = (lift_coeff[j]*LIFTED[j]) >> (19-shift);

        // replace first coeff by log of frame energy (which is in QN)
        // energy is QN
        LIFTED[0] = energy>>(QN-12);

#ifdef PRINTDEB
        DumpInt("LIFTED", LIFTED, NUMCEP, FrameCount, 12);
#endif
#ifdef PRINTINT
	if (CoreId==COREID) {
	  printf("dump lifted %d Frame %d\n",i,FrameCount);
        for (j=0; j<NUMCEP; j++) {
	  if (!(j%10)) printf("Id %d %d == ",CoreId, j);
            printf("%x ",  LIFTED[j]);
            if (!((j+1)%10)) printf("\n");
        }
	}
#endif
        // copy MFCC to feature list as short int 9.7 => 9.0
        for (j=0; j<NUMCEP; j++)
            FEAT_LIST[FrameCount*NUMCEP + j] = (short int) (LIFTED[j]>>(12));

#ifdef PERFF
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles 4 :%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif
	
        FrameCount++;
    }

#ifdef RUN_MFCC_ON_CLUST    
    CLUSTER_SynchBarrier(0);
#endif
    
    //#ifdef PRINTINT
    if (CoreId==0) {
      printf("feature table %dx%d\n",NUMCEP,N_FRAME);
      for (i=0;i<NUMCEP*N_FRAME;i++) {if (!(i%NUMCEP)) printf("%d:\t",i/NUMCEP); printf("%d, ",FEAT_LIST[i]); if (!((i+1)%NUMCEP)) printf("\n");}
     }
    //#endif
#ifdef RUN_MFCC_ON_CLUST    
    CLUSTER_SynchBarrier(0);
#endif
}
