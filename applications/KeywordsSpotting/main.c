/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CCN_PULP
  #include <stdio.h>
  #include <stdint.h>
#endif

#include "rt/rt_api.h"
#include "math.h"

#define CLUSTER_PWON         1
#define CLUSTER_PWDOWN       0
#define CLUSTER_GAP          0
#define CLUSTER_FLAG_DEFAULT 0

// supersede the default stack allocation(2048): cause a warning at compile
#ifdef STACK_SIZE
r#undef STACK_SIZE
#endif
#define STACK_SIZE 1024
//
#define MOUNT           1
#define UNMOUNT         0
#define CID             0
#ifndef NULL
  #define NULL ((void *) 0)
#endif



int finished = 0;

// Kernel function
#include "KernelLibStdTypes.h"

#ifdef RT_HAS_HWCE
#include "CnnKernels_hwce.h"
#define POWERCNN1_2   (6670.f)
#define POWERCNN1   (5600.f)
#else
#include "CnnKernels.h"
#define POWERCNN1_2   (28000.f)
#define POWERCNN1   (16800.f)
#endif
#include "CnnKernels_tileddense.h"

#define C0_NFEAT 32
#define C1_NFEAT 32
#define CLast_NFEAT 12

#define IMG_SIZE 98
#define BUF0_SIZE 20000
#define BUF1_SIZE 13000
//#define BUF0_SIZE 2000
//#define BUF1_SIZE 1300


// includes for MFCC feature
#include "MFCC.h"
#include "LUT.def"

// produced by read_data_test.c
#ifdef DOMFCC
#include "DataTest.h"
#else
#include "DataFEAT.h"
#endif


#ifdef RT_HAS_HWCE
#include "coeff_hwce.h"
#else
#ifdef FULL_PREC
#include "coeff_ref.h"
#endif

#ifdef W_HALFCHAR
#include "coeff.h"
#endif

#ifdef W_CHAR
#include "coeff.h"
#endif
#endif


// buffers for MFCC feature
//v2s  W_Frame[N_FFT+4];
//short int Frame[FRAME];
short int FEAT_LIST[N_FRAME*NUMCEP];
RT_FC_DATA v2s  *W_Frame;
RT_FC_DATA short int *Frame;
short int *InSignal = DataIn;
short int *pfeat_list;

// sqrt(2) to normalize dct 
#define NORMDCT  ((short int) floor(1.0/sqrt(2)*(1<<15)))		


#define CHECKSUM 52259

char *word_list[12] = { "silence", "none", "yes", "no", "up", "down", "right", "left", "on", "off", "stop",  "go"}   ;

extern Kernel_T AllKernels[3];

// L2 input, output & weights
#define qf 13



#define Abs(a) (((a)<0)?(-a):(a))

RT_L2_DATA  short int *l2_big0;
RT_L2_DATA  short int *l2_big1;


void cnn_process (rt_perf_t *cluster_perf) {
  
  unsigned int instr[3];
  unsigned int ElapsedTime[3];
  rt_perf_t *perf = cluster_perf;

  rt_perf_init(perf);

  // Configure performance counters for counting the cycles
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES)|(1<<RT_PERF_INSTR));
  
  rt_perf_reset(perf);
  rt_perf_start(perf);
  
#ifdef RT_HAS_HWCE
  Conv8x20MaxPool2x2_HWCE_0(pfeat_list,L2_W_0,l2_big0,8,L2_B_0,AllKernels + 0);
#else
  ConvLayer1(pfeat_list,L2_W_0,l2_big0,8,L2_B_0,8,20,AllKernels + 0);
#endif

  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[0] = rt_perf_get(perf, RT_PERF_INSTR);
  ElapsedTime[0] = rt_perf_get(perf,RT_PERF_CYCLES); 

  
#ifdef DUMP_LAYER
  table_print_data_t(pfeat_list,98*40,40,"conv0 input ");
  table_print_data_t(L2_B_0,32,32,"conv0 bias ");
  table_print_data_t(L2_W_0,32*8*20,8,"conv0 weight ");
  //table_print_data_ref(l2_big0,DataOut,32*16*39,16,"conv0 output ");
  table_print_data_t(l2_big0,16*39*32,16,"conv0 out ");
#endif

  rt_perf_reset(perf);
  rt_perf_start(perf);

#ifdef RT_HAS_HWCE
  Conv6x10_HWCE_1(l2_big0,L2_W_1,l2_big1,8,L2_B_1,AllKernels + 1);
#else

#ifdef W_HALFCHAR
  ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1); 
#endif
#ifdef FULL_PREC
  ConvLayer2(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1); 
#endif
#ifdef W_CHAR
  ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1); 
#endif

#endif

  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[1] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0];
  ElapsedTime[1] = rt_perf_get(perf,RT_PERF_CYCLES) - ElapsedTime[0]; 


  
#ifdef DUMP_LAYER2
  table_print_data_t(L2_B_1,32,32,"conv1 bias ");

#ifdef RT_HAS_HWCE
  table_print_data_t(l2_big1,32*14*30,14,"conv1 output ");
#else
  table_print_data_t(l2_big1,32*13*30,13,"conv1 output ");
#endif
  table_print_data_char(L2_W_1,4*10*32*32,4,"conv1 weight ");
#endif

  
  rt_perf_reset(perf);
  rt_perf_start(perf);

  // in,filter,normfilter,bias,normbias,out,outsize
#ifdef W_HALFCHAR
  Dense_halfchar(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2); 
#endif
#ifdef FULL_PREC
  Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2); 
#endif
#ifdef W_CHAR
  Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,12,AllKernels + 2); 
#endif

  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[2] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0] - instr[1];
  ElapsedTime[2] = rt_perf_get(perf,RT_PERF_CYCLES)- ElapsedTime[0] - ElapsedTime[1]; 
  
#ifdef DUMP_LAYER
  table_print_data_t(L2_B_2,12,12,"dense bias ");
#ifdef W_HALFCHAR
  table_print_data_char(L2_W_2,2*13*30*32,13,"dense weight ");
#endif
#ifdef FULL_PREC
  table_print_data_t(L2_W_2,2*13*30*32,13,"dense weight ");
#endif
  table_print_data_t(l2_big1,32*13*30,30,"dense input ");
#endif

#ifdef PRINT_PERF
  int TotalElapsed= ElapsedTime[0] +  ElapsedTime[1] +  ElapsedTime[2] ;
  printf("Layer0: %7d Cycles, %7d instruments\n", ElapsedTime[0], instr[0]);
  printf("Layer1: %7d Cycles, %7d instruments\n", ElapsedTime[1], instr[1]);
  printf("Layer2: %7d Cycles, %7d instruments\n", ElapsedTime[2], instr[2]);
  printf(" ============> CNN power consumption at 1.2V : %d µWatt (at 1V %d µWatt)\n\n", (int)((float)(1/(50000000.f/TotalElapsed)) * POWERCNN1_2), (int)((float)(1/(50000000.f/TotalElapsed)) * POWERCNN1  ));
#endif

} 



void table_print_data_ref(short int * pt, short int * ref, int dim, int packet, char *s)
{
  int i;
  int diff=0, diffmax=0, val=0;
  printf("dim=%d", dim);
  for(i=0;i<dim;i++)
  {
    if (!(i%packet)) printf("\nindex=%d %s\t", i, s);
    diff = Abs(*(pt+i) - *(ref+i));
    if (diff>diffmax) {diffmax=diff;val=*(pt+i);}
				   
    printf("%x(%x) ", *(pt+i), *(ref+i));
  }
  printf("DIFF MAX %x (%x)\n",diffmax,val);
  printf("\n\n");

}
void table_print_data_char( Word8 * pt, int dim, int packet, char *s)
{
  int i;
  printf("dim=%d", dim);
  for(i=0;i<dim;i++)
  {
    if (!(i%packet)) printf("\nindex=%d %s\t", i, s);
    printf("%x ", *(pt+i));
  }
  printf("\n\n");

}
void table_print_data_t(short int * pt, int dim, int packet, char *s)
{
  int i;
  printf("dim=%d", dim);
  for(i=0;i<dim;i++)
  {
    if (!(i%packet)) printf("\nindex=%d %s\t", i, s);
    printf("%x ", *(pt+i));
  }
  printf("\n\n");

}



static void cluster_main(rt_perf_t *perf)
{
  
#ifdef RT_HAS_HWCE
    /* Make HWCE event active */
    //printf("CNN launched on the HWCE\n");
    eu_evt_maskSet(1<<ARCHI_CL_EVT_ACC0);
#endif

#ifndef RT_HAS_HWCE
    //printf ("cluster master start\n");
    //printf("CNN  launched on cluster (%d cores)\n", rt_nb_pe());
  /* nb of cores for execution */
  unsigned int coreMask = (1 << rt_nb_pe()) - 1;
  gap8_setupbarrier(0, coreMask);
#endif



  cnn_process(perf);



}

static void end_of_app()
{
  finished = 1;
  printf ("End of application\n");
}


void compute_mfcc( ) {
	int i, j, k, Last;
	int FrameCount = 0;
	int FilterBanks = MFCC_BANK_CNT;
	short int lastsmp=0;
	 int maxin;
	short int shift;


	for (i=0; i<FRAME_STEP*N_FRAME; i+=FRAME_STEP) {
#ifdef PRINTDEB
	       DumpShortInt("Input", &InSignal[i], FRAME, FrameCount, Q15);
#endif
#ifdef PRINTINT
	       printf("dump input %d\n",i);
	       for (j=0; j<FRAME; j++) {
		 if (!(j%10)) printf("%d == ",j);
		 printf(" %x ", InSignal[j]);
		 if (!((j+1)%10)) printf("\n");
	       }
#endif

	       lastsmp = (i==0)?0:InSignal[i-1];
	       maxin=0;
	       for (j=0;j<FRAME;j++) if (Abs(InSignal[i+j])>maxin) maxin=Abs(InSignal[i+j]);
	       
	       shift = gap8_fl1(maxin);

	       if (shift<=13) shift = (15-shift-2); else shift=0;

#ifdef PRINTINT
	       printf("shift %d max %x\n",shift,maxin);
#endif
	       
	       PreEmphasis(InSignal+i , Frame, FRAME,lastsmp,shift);

#ifdef PRINTDEB
	       DumpShortInt("Premphasis Output", Frame, FRAME, FrameCount, Q15+shift);
#endif

	       WindowedFrame(Frame, W_Frame, HammingLUT, FRAME, N_FFT);

#ifdef PRINTDEB
		DumpComplex("Windowed", W_Frame, N_FFT, FrameCount, 15+shift);
#endif

#ifdef PRINTINT
		printf("dump windowed %d\n",i);
		for (j=0; j<N_FFT; j++) {
		  if (!(j%10)) printf("%d == ",j);
		  printf(" %x %x ", W_Frame[j][0], W_Frame[j][1]);
		  if (!((j+1)%10)) printf("\n");
		}
#endif
		
		Radix2FFT_DIF_args((short int*)W_Frame,  TwiddlesLUT, N_FFT);

		SwapSamples_args(W_Frame,SwapLUT,N_FFT);

#ifdef PRINTDEB
		DumpComplex("FFT", W_Frame, N_FFT, FrameCount, 7+shift);
#endif

	       maxin=0;
	       short int *W_FrameS=(short int*)W_Frame;

	       for (j=0;j<N_FFT;j++) if (Abs(W_FrameS[j])>maxin) maxin=Abs(W_FrameS[j]);

	       short int shift1 = gap8_fl1(maxin);


	       // 9.7
	       EstimatePower_int(W_Frame, HALF_FFT, N_FFT);

		 unsigned int* FramePower = ( unsigned int *) W_Frame;
#ifdef PRINTINT
		printf("dump power %d\n",i);
		for (j=0; j<N_FFT; j++) {
		  if (!(j%10)) printf("%d == ",j);
		  printf(" %x ", FramePower[j]);
		  if (!((j+1)%10)) printf("\n");
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

		energy = logfp(energy>>(2*shift)) + (QN-14-9)*gaplog2;

		unsigned int * MFCC_BASE = (unsigned int*) FramePower;
		ComputeMFCC_int(FramePower, MFCC_BASE);


#ifdef PRINTINT
		printf("dump MFCC %d\n",i);
		for (j=0; j<MFCC_BANK_CNT; j++) {
		  if (!(j%10)) printf("%d == ",j);
		  printf(" %x ", MFCC_BASE[j]);
		  if (!((j+1)%10)) printf("\n");
		}
#endif
#ifdef PRINTDEB
		// coeff 6.10, data 18.14  * 1/512 => 26
		DumpInt_norm("Filtered power", MFCC_BASE, MFCC_BANK_CNT, FrameCount, 9+24+2*shift,MFCC_FilterBank  );
#endif
		
		// 8.24
		Compute_log(MFCC_BASE);

		for (j=0;j<MFCC_BANK_CNT;j++) {
		  MFCC_BASE[j] -= 2*shift*gaplog2;
		}
		
#ifdef PRINTDEB
		DumpInt("LOG Filtered Power", MFCC_BASE, MFCC_BANK_CNT, FrameCount, QN);
#endif
		maxin=0;
		for (j=0;j<MFCC_BANK_CNT;j++)   if (Abs((int)MFCC_BASE[j])>maxin) maxin=Abs((int)MFCC_BASE[j]);
		shift = gap8_fl1(maxin);


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
		printf("dump input dct %d\n",i);
		for (j=0; j<2*NDCT; j++) {
		  if (!(j%10)) printf("%d == ",j);
		  printf(" %x %x ", in_dct[j][0],in_dct[j][1]);
		  if (!((j+1)%10)) printf("\n");
		}
#endif
		Compute_dct(in_dct, (v2s *)twidend,  (v2s *)TwiddlesLUT_128, SwapLUT_128);


		// NORMDCT Q15
		in_dct[0][0] = (in_dct[0][0] * NORMDCT) >> 15;
		for(j=0;j<64;j++) in_dct[j][0] = (in_dct[j][0] * NORMDCT) >> 15;
		
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
		printf("dump lifted %d\n",i);
		for (j=0; j<NUMCEP; j++) {
		  if (!(j%10)) printf("%d == ",j);
		  printf(" %x ", LIFTED[j]);
		  if (!((j+1)%10)) printf("\n");
		}
#endif
		// copy MFCC to feature list as short int 9.7 => 9.0
		for (j=0; j<NUMCEP; j++)
		  FEAT_LIST[FrameCount*NUMCEP + j] = (short int) (LIFTED[j]>>(12));

		
		FrameCount++;


	}




}


int main()
{
#define Max(a, b)	(((a)>=(b))?(a):(b))
 char error =0;
 int j;
 
 rt_perf_t *cluster_perf;
 rt_event_sched_t sched;
 rt_event_sched_init(&sched);
 if (rt_event_alloc(&sched, 8)) return -1;


 rt_event_t *event = rt_event_get_blocking(NULL);
 rt_cluster_mount(CLUSTER_PWON, CLUSTER_GAP, 0, NULL);

 void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
 if (stacks == NULL) return -1;



 rt_perf_t *perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
 if (perf == NULL) return -1;

 //======================MFCC
 Frame = rt_alloc(RT_ALLOC_FC_DATA,FRAME*sizeof(short int));
 W_Frame = rt_alloc(RT_ALLOC_FC_DATA,(N_FFT+4)*sizeof(v2s));
 
#ifdef DOMFCC 
 rt_perf_init(perf);
 rt_perf_conf(perf, (1<<RT_PERF_CYCLES));
 rt_perf_reset(perf);
 rt_perf_start(perf);
 compute_mfcc();
 rt_perf_save(perf);
 rt_perf_stop(perf);
 printf("MFCC done, features generated.\n\n\n");
 //printf("\n\n ============> cycles %d\n\n",rt_perf_get(perf, RT_PERF_CYCLES));
 pfeat_list = (short int*) &(FEAT_LIST[0]);
#ifdef PRINTFEAT
 printf("dump features %d\n",NUMCEP*N_FRAME);
 for (j=0; j<NUMCEP*N_FRAME; j++) {
   //if (!(j%20)) printf("%d == ",j);
   printf(" %d, ", FEAT_LIST[j]);
   if (!((j+1)%20)) printf("\n");
 }
#endif

#else
 printf("CNN on datatest.h\n");
 pfeat_list = (short int*) &(DataIn[0]);

#endif

 cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
 if (cluster_perf == NULL) return -1;


 rt_cluster_mount(CLUSTER_PWDOWN, CLUSTER_GAP, 0, NULL);

   
#ifdef RT_HAS_HWCE
   printf("start kws HWCE\n");
#else
   printf("start kws SW\n");
#endif


   
   
   //======================CNN
   // Allocate the memory of L2 for the performance structure
   rt_cluster_mount(CLUSTER_PWON, CLUSTER_GAP, 0, NULL);
   
   // allocate data buffers
   l2_big0 = rt_alloc(RT_ALLOC_L2_CL_DATA, BUF0_SIZE*sizeof(short int));
   l2_big1 = rt_alloc(RT_ALLOC_L2_CL_DATA, BUF1_SIZE*sizeof(short int));
  
    L1_Memory  = rt_alloc(RT_ALLOC_CL_DATA, _L1_Memory_SIZE_big) ;
    if(L1_Memory == NULL) {
      printf("WorkingArea alloc error\n");
      return -1;
    }
   // starts up the cluster
   //rt_event_t *pippo = rt_event_get_blocking(NULL);
   rt_cluster_call(NULL, CID, (void *)cluster_main, cluster_perf, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);
   //rt_event_wait(pippo);

   rt_free(RT_ALLOC_CL_DATA,(void *) L1_Memory,  _L1_Memory_SIZE_big);

   
   rt_cluster_mount(CLUSTER_PWDOWN, CLUSTER_GAP, 0, NULL);


  
 
 {
     //  ******************************** Softmax on FC ****************************
     int i;
     int sum = 0;
     char *s;
     int j;

     printf("\n");
     int max=0x80000000;
     unsigned char idx_max=0;

     for(i=0;i<CLast_NFEAT; i++){
       printf("\t%x  \n",  l2_big0[i]);
       l2_big0[i] = l2_big0[i] ;
       printf(" feat %d: %d  \n", i, l2_big0[i]);
       sum += l2_big0[i];
#ifndef TESTNONE
       if (l2_big0[i]>max && i!=1) {max=l2_big0[i];idx_max=i;}
#else
       if (l2_big0[i]>max) {max=l2_big0[i];idx_max=i;}
#endif
     }
 

     printf("\nfound max %d\n",idx_max+1);
     printf("\nfound word %s\n",word_list[idx_max]);

     rt_free(RT_ALLOC_L2_CL_DATA, (void *)l2_big0, BUF0_SIZE*sizeof(short int));
     rt_free(RT_ALLOC_L2_CL_DATA, (void *)l2_big1, BUF1_SIZE*sizeof(short int));
     
#ifdef CHECK_CHECKSUM
     if((sum == CHECKSUM)) {
       printf("CHECKSUM OK!!! (%d)\n", sum);
     }
     else {
       printf("CHECKSUM WRONG!!! (%d instead of %d)\n\n", sum, CHECKSUM);
       error = 1;
     }
#endif
 }
   return error;
#undef Max
}
