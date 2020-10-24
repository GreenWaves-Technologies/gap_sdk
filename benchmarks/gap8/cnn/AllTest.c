#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pulp.h"
#include "rt/rt_api.h"

#define NOGPIO
#define BYTE 


#define ITERATIONS 100
#define ENABLE_CYCLE_TRACE 0

#define ALIM_1_VOLT 0
#define FREQ_FC (250*1000000)
#define FREQ_CL (175*1000000)


#define Wi  112  
#define Hi  112  
#define Wic 100  
#define Hic 100  
#define Wil 1024
#define Hil 16   

#define Wxor    100
#define Hxor    100

char tests_names[][50]={
{"2x2/2 Max Pool"},
{"2x2/2 Avg Pool"},
{"5x5 Convolutions"},
{"Linear"},
{"Xnor Conv 5x5"},

{"2x2/2 Max Pool Vect"},
{"2x2/2 Avg Pool Vect"},
{"5x5 Convolutions Vect"},
{"LinearVect"},

{"2x2/2 Parallel Max Pool"},
{"2x2/2 Parallel Avg Pool"},
{"Parallel 5x5 Convolution"},
{"Parallel Linear"},
{"Parallel Xnor Conv 5x5"},

{"2x2/2 Parallel Max Pool Vect"},
{"2x2/2 Parallel Avg Pool Vect"},
{"Parallel Convolution Vect"},
{"Parallel Linear Vect"}
};

int test_input_w[]={Wi,Wi,Wi,Wil,Wxor};
int test_input_h[]={Hi,Hi,Hi,Hil,Hxor};


int tests_ops[10]={
    ((Wi/2)*(Hi/2)),
    ((Wi/2)*(Hi/2)),
    (Wic-4)*(Hic-4),
    (Wil*Hil),
    (Wic-4)*(Hic-4)
};

int tests_input[][2]={
    {Wi , Hi },
    {Wi , Hi },
    {Wic, Hic},
    {Wil, Hil},
    {Wic, Hic}
};


char tests_titles[][50]={
{"Sequential"},
{"Vector"},
{"Parallel"},
{"Parallel Vector"},
};



#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#ifndef RISCV
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

#ifdef __EMUL__
#define plp_cluster_fetch(a)
#define plp_cluster_wait(a)
#endif



#ifdef NOGPIO
#define WriteGpio(a, b)
#else
#define WriteGpio(a, b) rt_gpio_set_pin_value(0, a, b)
#endif

#ifdef RISCV
#define TOT_TEST 1
int test_num[TOT_TEST]={5};

#ifndef __EMUL__
#endif


#define L2_MEM                          __attribute__((section(".heapl2ram")))
#define L1_CL_MEM                       __attribute__((section(".heapsram")))
#define L1_FC_MEM                       __attribute__((section(".fcTcdm")))
/* HW timer */
#define ARCHI_FC_TIMER_ADDR                     ( ARCHI_FC_PERIPHERALS_ADDR + ARCHI_FC_TIMER_OFFSET  )
#define PLP_TIMER_VALUE_LO                      0x08
#define PLP_TIMER_CFG_REG_LO                    0x00
#define PLP_TIMER_ENABLE_BIT            0
#define PLP_TIMER_RESET_BIT             1
#define PLP_TIMER_IRQ_ENABLE_BIT        2
#define PLP_TIMER_IEM_BIT               3
#define PLP_TIMER_CMP_CLR_BIT           4
#define PLP_TIMER_ONE_SHOT_BIT          5
#define PLP_TIMER_PRESCALER_ENABLE_BIT  6
#define PLP_TIMER_CLOCK_SOURCE_BIT      7
#define PLP_TIMER_PRESCALER_VALUE_BIT   8
#define PLP_TIMER_PRESCALER_VALUE_BITS  8
#define PLP_TIMER_64_BIT                31

#define plp_timer_conf_get(a,b,c,d,e,f,g,h,i)      ((a << PLP_TIMER_ENABLE_BIT) \
        | (b << PLP_TIMER_RESET_BIT) \
        | (c << PLP_TIMER_IRQ_ENABLE_BIT) \
        | (d << PLP_TIMER_IEM_BIT) \
        | (e << PLP_TIMER_CMP_CLR_BIT) \
        | (f << PLP_TIMER_ONE_SHOT_BIT) \
        | (g << PLP_TIMER_PRESCALER_ENABLE_BIT) \
        | (h << PLP_TIMER_PRESCALER_VALUE_BIT) \
        | (i << PLP_TIMER_64_BIT) \
        )
#define gap8_resethwtimer()                     pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_LO, plp_timer_conf_get(1,1,0,0,0,0,0,0,0))
#define gap8_readhwtimer()                      pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_LO)

#else
#define TOT_TEST 4
int test_num[TOT_TEST]={5,4,5,4};
#include "Gap8.h"

static int CoreCountDynamic = 0;
static int ActiveCore = 8;

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}
#endif

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0


typedef struct ClusterArg{
    int test_num;
    int Iter;
    int Trace;
    char Mode[10];
    int Iter_operations;
} ClusterArg_t;

ClusterArg_t Arg;


char str[100];
static char *float_to_string(float in){
     
    int d1 = in;                
    float f2 = in - d1;         
    int d2 = trunc(f2 * 10000); 
 
    sprintf (str, "%d.%04d", d1, d2);
    return str;
}

#ifndef RISCV
#define B_ins(dst, src, size, off)      gap8_bitinsert(dst, src, size, off)
#define B_ins_r(dst, src, size, off)    gap8_bitinsert_r(dst, src, size, off)
#define B_ext(x, size, off)             gap8_bitextract(x, size, off)
#define B_extu(x, size, off)            gap8_bitextractu(x, size, off)
#define B_ext_r(x, size, off)           gap8_bitextract_r(x, size, off)
#define B_extu_r(x, size, off)          gap8_bitextractu_r(x, size, off)
#define B_popc(src)                     __builtin_popcount((src))
#else
static __attribute__ ((always_inline)) unsigned int bitcount32(unsigned int b)
{
        b -= (b >> 1) & 0x55555555;
        b = (b & 0x33333333) + ((b >> 2) & 0x33333333);
        b = (b + (b >> 4)) & 0x0f0f0f0f;
        return (b * 0x01010101) >> 24;
}
#define B_ins(dst, src, size, off)      (((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define B_ins_r(dst, src, size, off)    (((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define B_ext(x, size, off)             (((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define B_extu(x, size, off)            (((x)>>(off))&((unsigned int)(1<<(size))-1))
#define B_ext_r(x, size, off)           (((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define B_extu_r(x, size, off)          (((x)>>(off))&((unsigned int)(1<<(size))-1))
#define B_popc(src)                     bitcount32((src))
#endif

#define VSOC	1000
#define GPIO	17

#ifdef BYTE
typedef signed char Ty;
#else
typedef short int Ty;
#endif

#ifdef BYTE
#define MAX_MEM	55000
#else
#define MAX_MEM	(55000/2)
#endif
Ty L1_CL_MEM Mem[MAX_MEM];

typedef struct {
	Ty *__restrict__ In;
	int W;
	int H;
	Ty *__restrict__ Filter;
	Ty *__restrict__ Out;
	int Norm;
} ArgConvT;

typedef struct {

        unsigned int InBit;
        signed char *__restrict__ Out;
        unsigned int FilterBit;
        int W;
        int H;
} ArgConvBT;

void CheckMem(int Size)

{
	if (Size>MAX_MEM) {
		printf("Memory Overflow (%d>%d). Aborting\n", Size, MAX_MEM); exit(0);
	}
}

#ifndef RISCV
v4s L1_CL_MEM LinearMask[4] = {(v4s) 0, (v4s) 0xFF, (v4s) 0xFFFF, (v4s) 0xFFFFFF};
#ifdef BYTE
void __attribute__ ((noinline)) MaxPoolingVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) {
			v4s V0 = (v4s)(int)(*((short *)(In+2*l*W+2*c))), V1 = (v4s)(int)(*((short *)(In+(2*l+1)*W+2*c)));
			V0 = gap8_max4(V0, V1);
			Out[l*Wo+c] = Max(V0[0], V0[1]);
		}
}

void __attribute__ ((noinline)) ParMaxPoolingVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++) {
                        v4s V0 = (v4s)(int)(*((short *)(In+2*l*W+2*c))), V1 = (v4s)(int)(*((short *)(In+(2*l+1)*W+2*c)));
                        V0 = gap8_max4(V0, V1);
                        Out[l*Wo+c] = Max(V0[0], V0[1]);
                }
        gap8_waitbarrier(0);
}


void __attribute__ ((noinline)) AvgPoolingVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) {
			v4s V0 = (v4s)(int)(*((short *)(In+2*l*W+2*c))), V1 = (v4s)(int)(*((short *)(In+(2*l+1)*W+2*c)));
			V0 = __builtin_shuffle(V0, V1, (v4s){0,1,4,5});
			Out[l*Wo+c] = gap8_dotp4(V0, ((v4s){1,1,1,1}))>>2;
		}
}

void __attribute__ ((noinline)) ParAvgPoolingVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++) {
                        v4s V0 = (v4s)(int)(*((short *)(In+2*l*W+2*c))), V1 = (v4s)(int)(*((short *)(In+(2*l+1)*W+2*c)));
                        V0 = __builtin_shuffle(V0, V1, (v4s){0,1,4,5});
                        Out[l*Wo+c] = gap8_dotp4(V0, ((v4s){1,1,1,1}))>>2;
                }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) Additive5x5ConvolutionVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        v4s C0  = *((v4s *) &Filter[0]),
            C1  = *((v4s *) &Filter[5]),
            C2  = *((v4s *) &Filter[10]),
            C3  = *((v4s *) &Filter[15]),
            C4  = *((v4s *) &Filter[20]),
            C5 = gap8_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
            C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

        v4s V0, V1, V2, V3, V4, V5, V6;
        v4s Mask  = {1,2,3,4};

        signed char *PtO1 = Out;

        for (int w=0; w<(W-4); w++) {
                v4s *PtI = (v4s *) (In + w);
                signed char *PtO = PtO1;
                int x0,x1,x2,x3;
                V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V5 = gap8_pack4(x0,x1,x2,x3);
                for (int h=0; h<(H-4); h++) {
                        int S = *PtO<<Norm;
                        V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
                        S = gap8_sumdotp4(V0,  C0,  S); S = gap8_sumdotp4(V1,  C1,  S);
                        S = gap8_sumdotp4(V2,  C2,  S); S = gap8_sumdotp4(V3,  C3,  S);
                        S = gap8_sumdotp4(V4,  C4,  S); S = gap8_sumdotp4(V5,  C5,  S); S = gap8_sumdotp4(V6,  C6,  S);
                        V0 = V1; V1 = V2; V2 = V3; V3 = V4;
                        V5 = __builtin_shuffle(V5, V6, Mask);
                        *PtO = S>>Norm; PtO+=(W-4);
                }
                PtO1++;
        }
}

void __attribute__ ((noinline)) ParAdditive5x5ConvolutionVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;

        v4s C0  = *((v4s *) &Filter[0]),
            C1  = *((v4s *) &Filter[5]),
            C2  = *((v4s *) &Filter[10]),
            C3  = *((v4s *) &Filter[15]),
            C4  = *((v4s *) &Filter[20]),
            C5 = gap8_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
            C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

        v4s V0, V1, V2, V3, V4, V5, V6;
        v4s Mask  = {1,2,3,4};
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(W-4);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, W-4);
        signed char *PtO1 = Out+First;

        for (unsigned int w=First; w<Last; w++) {
                v4s *PtI = (v4s *) (In+w);
                signed char *PtO = PtO1;
                int x0,x1,x2,x3;
                V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
                V5 = gap8_pack4(x0,x1,x2,x3);
                for (int h=0; h<(H-4); h++) {
                        int S = *PtO<<Norm;
                        V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
                        S = gap8_sumdotp4(V0,  C0,  S); S = gap8_sumdotp4(V1,  C1,  S);
                        S = gap8_sumdotp4(V2,  C2,  S); S = gap8_sumdotp4(V3,  C3,  S);
                        S = gap8_sumdotp4(V4,  C4,  S); S = gap8_sumdotp4(V5,  C5,  S); S = gap8_sumdotp4(V6,  C6,  S);
                        V0 = V1; V1 = V2; V2 = V3; V3 = V4;
                        V5 = __builtin_shuffle(V5, V6, Mask);
                        *PtO = S>>Norm; PtO+=(W-4);
                }
                PtO1++;
        }
        gap8_waitbarrier(0);

}

void __attribute__ ((noinline)) LinearVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        for (int i=0; i<H; i++) {
                int R = Out[i]<<Norm;
                v4s *F = (v4s *)(Filter+W*i);
                v4s *I = (v4s *)In;
                for (int j=0; j<(W>>3); j++) {
                        R = gap8_sumdotp4(I[2*j], F[2*j], R);
                        R = gap8_sumdotp4(I[2*j+1], F[2*j+1], R);
                }
		// if ((W&0x3)!=0) R = gap8_sumdotp4(I[W>>2]&LinearMask[W&0x3], F[W>>2], R);
                for (int j=8*(W>>3); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
}

void __attribute__ ((noinline)) ParLinearVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(H);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, H);

        for (unsigned int i=First; i<Last; i++) {
                int R = Out[i]<<Norm;
                v4s *F = (v4s *)(Filter+W*i);
                v4s *I = (v4s *)In;
                for (int j=0; j<(W>>3); j++) {
                        R = gap8_sumdotp4(I[2*j], F[2*j], R);
                        R = gap8_sumdotp4(I[2*j+1], F[2*j+1], R);
                }
		// if ((W&0x3)!=0) R = gap8_sumdotp4(I[W>>2]&LinearMask[W&0x3], F[W>>2], R);
                for (int j=8*(W>>3); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
        gap8_waitbarrier(0);
}

#else
void __attribute__ ((noinline)) MaxPoolingVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) {
			v2s V0 = *((v2s *)(In+2*l*W+2*c)), V1 = *((v2s *)(In+(2*l+1)*W+2*c));
			V0 = gap8_max2(V0, V1);
			Out[l*Wo+c] = Max(V0[0], V0[1]);
		}
}

void __attribute__ ((noinline)) ParMaxPoolingVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++) {
                        v2s V0 = *((v2s *)(In+2*l*W+2*c)), V1 = *((v2s *)(In+(2*l+1)*W+2*c));
                        V0 = gap8_max2(V0, V1);
                        Out[l*Wo+c] = Max(V0[0], V0[1]);
                }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) AvgPoolingVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) {
			v2s V0 = *((v2s *)(In+2*l*W+2*c)), V1 = *((v2s *)(In+(2*l+1)*W+2*c));
			int R=gap8_dotp2(V0, ((v2s){1,1}));
			Out[l*Wo+c] = gap8_sumdotp2(V0, ((v2s){1,1}), R)>>2;
		}
}

void __attribute__ ((noinline)) ParAvgPoolingVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++) {
                        v2s V0 = *((v2s *)(In+2*l*W+2*c)), V1 = *((v2s *)(In+(2*l+1)*W+2*c));
                        int R=gap8_dotp2(V0, ((v2s){1,1}));
                        Out[l*Wo+c] = gap8_sumdotp2(V0, ((v2s){1,1}), R)>>2;
                }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) Additive5x5ConvolutionVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
            C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
            C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
            C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
            C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
            C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
            C12 = gap8_pack2(Filter[24], 0);

        v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
        v2s Mask  = {1,2};

        short int *PtO1 = Out;

        for (int w=0; w<(W-4); w++) {
                v2s *PtI = (v2s *) (In + w);
                short int *PtO = PtO1;
                int X, Y;
                V0 = *PtI++; V1 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
                V2 = *PtI++; V3 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V10 = gap8_pack2(X, Y);
                V4 = *PtI++; V5 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
                V6 = *PtI++; V7 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V11 = gap8_pack2(X, Y);
                for (int h=0; h<H-4; h++) {
                        V8 = *PtI++; V9 = *PtI++; V12 = *PtI; PtI = (v2s*) ((short int *)PtI+W-4);
                        int S = *PtO<<Norm;
                        S = gap8_sumdotp2(V0,  C0,  S); S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V10, C10, S);
                        S = gap8_sumdotp2(V2,  C2,  S); S = gap8_sumdotp2(V3,  C3,  S);
                        S = gap8_sumdotp2(V4,  C4,  S); S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V11, C11, S);
                        S = gap8_sumdotp2(V6,  C6,  S); S = gap8_sumdotp2(V7,  C7,  S);
                        S = gap8_sumdotp2(V8,  C8,  S); S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V12, C12, S);
                        S =  S>>Norm;
                        V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
                        V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
                        *PtO = S; PtO+=(W-4);
                }
                PtO1++;
        }
}

void __attribute__ ((noinline)) ParAdditive5x5ConvolutionVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;
        v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
            C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
            C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
            C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
            C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
            C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
            C12 = gap8_pack2(Filter[24], 0);

        v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
        v2s Mask  = {1,2};
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(W-4);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, W-4);
        short int *PtO1 = Out+First;

        for (unsigned int w=First; w<Last; w++) {
                v2s *PtI = (v2s *) (In+w);
                short int *PtO = PtO1;
                int X, Y;
                V0 = *PtI++; V1 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
                V2 = *PtI++; V3 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V10 = gap8_pack2(X, Y);
                V4 = *PtI++; V5 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
                V6 = *PtI++; V7 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V11 = gap8_pack2(X, Y);
                for (int h=0; h<H-4; h++) {
                        V8 = *PtI++; V9 = *PtI++; V12 = *PtI; PtI = (v2s*) ((short int *)PtI+W-4);
                        int S = *PtO<<Norm;
                        S = gap8_sumdotp2(V0,  C0,  S); S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V10, C10, S);
                        S = gap8_sumdotp2(V2,  C2,  S); S = gap8_sumdotp2(V3,  C3,  S);
                        S = gap8_sumdotp2(V4,  C4,  S); S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V11, C11, S);
                        S = gap8_sumdotp2(V6,  C6,  S); S = gap8_sumdotp2(V7,  C7,  S);
                        S = gap8_sumdotp2(V8,  C8,  S); S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V12, C12, S);
                        S =  S>>Norm;
                        V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
                        V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
                        *PtO = S; PtO+=(W-4);
                }
                PtO1++;
        }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) LinearVect(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        for (int i=0; i<H; i++) {
                int R = Out[i]<<Norm;
                v2s *F = (v2s *)(Filter+W*i);
                v2s *I = (v2s *)In;
                for (int j=0; j<(W>>2); j++) {
                        R = gap8_sumdotp2(I[2*j], F[2*j], R);
                        R = gap8_sumdotp2(I[2*j+1], F[2*j+1], R);
                }
                for (int j=4*(W>>2); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
}

void __attribute__ ((noinline)) ParLinearVect(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(H);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, H);

        for (unsigned int i=First; i<Last; i++) {
                int R = Out[i]<<Norm;
                v2s *F = (v2s *)(Filter+W*i);
                v2s *I = (v2s *)In;
                for (int j=0; j<(W>>2); j++) {
                        R = gap8_sumdotp2(I[2*j], F[2*j], R);
                        R = gap8_sumdotp2(I[2*j+1], F[2*j+1], R);
                }
                for (int j=4*(W>>2); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
        gap8_waitbarrier(0);
}
#endif

void __attribute__ ((noinline)) ParMaxPooling(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++)
                        Out[l*Wo+c] = Max(Max(In[2*l*W+2*c], In[2*l*W + 2*c+1]), Max(In[(2*l+1)*W+2*c], In[(2*l+1)*W + 2*c+1]));
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) ParAvgPooling(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H;
        int Wo=W/2, Ho=H/2;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);

        for (unsigned int c=First; c<Last; c++)
                for (int l=0; l<Ho; l++)
                        Out[l*Wo+c] = (In[2*l*W+2*c]+In[2*l*W + 2*c+1]+In[(2*l+1)*W+2*c]+In[(2*l+1)*W + 2*c+1])>>2;
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) ParAdditive5x5Convolution(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;
        int FH=5,FW=5;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(W-4);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, W-4);
        for (unsigned int c=First; c<Last; c++) {
                for (int l=0; l<(H-4); l++) {
                        int R = Out[l*W+c]<<Norm;
                        for (int kl=0; kl<FH; kl++) {
                                R += Filter[kl*FW+0]*In[(l+kl)*W + c+0];
                                R += Filter[kl*FW+1]*In[(l+kl)*W + c+1];
                                R += Filter[kl*FW+2]*In[(l+kl)*W + c+2];
                                R += Filter[kl*FW+3]*In[(l+kl)*W + c+3];
                                R += Filter[kl*FW+4]*In[(l+kl)*W + c+4];
/*
                                for (int kc=0; kc<FW; kc++) {
                                        R += Filter[kl*FW+kc]*In[(l+kl)*W + c+kc];
                                }
*/
                        }
                        Out[l*W+c] = R>>Norm;
                }
        }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) ParLinear(ArgConvT *Arg)

{
        Ty *__restrict__ In = Arg->In;
        Ty *__restrict__ Filter = Arg->Filter;
        Ty *__restrict__ Out = Arg->Out;
        int W=Arg->W, H=Arg->H, Norm=Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(H);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, H);

        for (unsigned int i=First; i<Last; i++) {
                int R = Out[i]<<Norm;
                for (int j=0; j<(W>>2); j++) {
                        R += In[4*j]*Filter[W*i+4*j];
                        R += In[4*j+1]*Filter[W*i+4*j+1];
                        R += In[4*j+2]*Filter[W*i+4*j+2];
                        R += In[4*j+3]*Filter[W*i+4*j+3];
                }
                for (int j=4*(W>>2); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
        gap8_waitbarrier(0);
}

void __attribute__ ((noinline)) ParXnorConv5x5(ArgConvBT *Arg)

{
        unsigned int InBit = Arg->InBit;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int FilterBit = Arg->FilterBit;
        int W = Arg->W;
        int H = Arg->H;

        int Wo = W-4;
	int Ho = H-4;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(Wo);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Wo);
        int Wo_F=First; int Wo_L=Last;
        int Ho_F=0; int Ho_L=Ho;
        unsigned int Stride=1, K=2;
        unsigned int C = *((unsigned int *) (FilterBit/8))>>(FilterBit%8);
        signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
        unsigned char *PtByte;
        unsigned int PtBit;
        unsigned int ExtMask=5<<5;
        unsigned int CC = C;
        CC = B_ins(CC, B_ext(C, 5,  5), 5,  6);
        CC = B_ins(CC, B_ext(C, 5, 10), 5, 12);
        CC = B_ins(CC, B_ext(C, 5, 15), 5, 18);
        CC = B_ins(CC, B_ext(C, 5, 20), 5, 24);

        int Iter = Wo_L-Wo_F;
        for ( int i=0, w=Wo_F; (i<(Iter/2)); i++, w+=2) {
                unsigned int V, N;
                PtBit = InBit+(Ho_F*Stride)*W + (w*Stride);
                PtByte = (unsigned char *) (PtBit/8);
                char *PtO = (char *) PtO1;
                V = 0;
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6,  0); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6,  6); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6, 12); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6, 18); PtBit += W; PtByte = (unsigned char *) (PtBit/8);

                for (int h=Ho_F; h<Ho_L; h++) {
                        N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                        V = B_ins(V, N, 6, 24); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                        int R0 = B_popc((~(V^CC))&0x1F7DF7DF),
                            R1 = B_popc((~((V>>1)^CC))&0x1F7DF7DF);
                        unsigned int V1 = R0|(R1<<8);
                        v4s Val = (v4s) (unsigned int) (*(unsigned short int *) PtO);
                        Val = __builtin_pulp_add4(Val, (v4s) V1);
                        *(unsigned short int *)PtO = (unsigned short int)(unsigned int)Val; PtO+=Wo;
                        V = V>>((2*K+1)+1);
                }
                PtO1+=2;
        }
        PtO1 = Out+Wo*Ho_F+Wo_F+2*(Iter/2);
        for (int w=Wo_F+2*(Iter/2); w<Wo_L; w++) {
                unsigned int V, N;
                PtBit = InBit+(Ho_F*Stride)*W + (w*Stride);
                PtByte = (unsigned char *) (PtBit/8);
                char *PtO = (char *) PtO1;
                V = 0;
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  0); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  5); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  10); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  15); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                for (int h=Ho_F; h<Ho_L; h++) {
                        N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                        V = B_ins(V, N, 5,  20); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                        *PtO += B_popc((~(V^C))&0x1FFFFFF); PtO+=Wo;
                        V = V>>(2*K+1);
                }
                PtO1++;
        }
}

#endif


void __attribute__ ((noinline)) MaxPooling(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) 
			Out[l*Wo+c] = Max(Max(In[2*l*W+2*c], In[2*l*W + 2*c+1]), Max(In[(2*l+1)*W+2*c], In[(2*l+1)*W + 2*c+1]));
}

void __attribute__ ((noinline)) AvgPooling(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Out)

{
	int Wo=W/2, Ho=H/2;
	for (int c=0; c<Wo; c++) 
		for (int l=0; l<Ho; l++) 
			Out[l*Wo+c] = (In[2*l*W+2*c]+In[2*l*W + 2*c+1]+In[(2*l+1)*W+2*c]+In[(2*l+1)*W + 2*c+1])>>2;
}

void __attribute__ ((noinline)) Additive5x5Convolution(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        int FH=5,FW=5;
        for (int c=0; c<(W-4); c++) {
                for (int l=0; l<(H-4); l++) {
                        int R = Out[l*W+c]<<Norm;
                        for (int kl=0; kl<FH; kl++) {
                                R += Filter[kl*FW+0]*In[(l+kl)*W + c+0];
                                R += Filter[kl*FW+1]*In[(l+kl)*W + c+1];
                                R += Filter[kl*FW+2]*In[(l+kl)*W + c+2];
                                R += Filter[kl*FW+3]*In[(l+kl)*W + c+3];
                                R += Filter[kl*FW+4]*In[(l+kl)*W + c+4];
/*
                                for (int kc=0; kc<FW; kc++) {
                                        R += Filter[kl*FW+kc]*In[(l+kl)*W + c+kc];
                                }
*/
                        }
                        Out[l*W+c] = R>>Norm;
                }
        }
}

void __attribute__ ((noinline)) Linear(Ty *__restrict__ In, int W, int H, Ty *__restrict__ Filter, Ty *__restrict__ Out, int Norm)

{
        for (int i=0; i<H; i++) {
                int R = Out[i]<<Norm;
                for (int j=0; j<(W>>2); j++) {
                        R += In[4*j]*Filter[W*i+4*j];
                        R += In[4*j+1]*Filter[W*i+4*j+1];
                        R += In[4*j+2]*Filter[W*i+4*j+2];
                        R += In[4*j+3]*Filter[W*i+4*j+3];
                }
                for (int j=4*(W>>2); j<W; j++) R += In[j]*Filter[W*i+j];
                Out[i] = R>>Norm;
        }
}

void __attribute__ ((noinline)) XnorConv5x5(
        unsigned int InBit,
        signed char *__restrict__ Out,
        unsigned int FilterBit,
        int W,
        int H
        )
/*
        int Wo,
        int Wo_F,
        int Wo_L,
        int Ho,
        int Ho_F,
        int Ho_L
        )
*/

{
        int Wo = W-4; int Ho = H-4;
        int Wo_F=0; int Wo_L=Wo;
        int Ho_F=0; int Ho_L=Ho;

        unsigned int Stride=1, K=2;
        unsigned int C = *((unsigned int *) (FilterBit/8))>>(FilterBit%8);
        signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
        unsigned char *PtByte;
        unsigned int PtBit;
        unsigned int ExtMask=5<<5;
        unsigned int CC = C;
        CC = B_ins(CC, B_ext(C, 5,  5), 5,  6);
        CC = B_ins(CC, B_ext(C, 5, 10), 5, 12);
        CC = B_ins(CC, B_ext(C, 5, 15), 5, 18);
        CC = B_ins(CC, B_ext(C, 5, 20), 5, 24);

        int Iter = Wo_L-Wo_F;
        for (int i=0, w=Wo_F; (i<(Iter/2)); i++, w+=2) {
                unsigned int V, N;
                PtBit = InBit+(Ho_F*Stride)*W + (w*Stride);
                PtByte = (unsigned char *) (PtBit/8);
                char *PtO = (char *) PtO1;
                V = 0;
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6,  0); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6,  6); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6, 12); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                V = B_ins(V, N, 6, 18); PtBit += W; PtByte = (unsigned char *) (PtBit/8);

                for (int h=Ho_F; h<Ho_L; h++) {
                        N = B_extu_r(*(unsigned short int *)PtByte, 6, (PtBit%8));
                        V = B_ins(V, N, 6, 24); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                        int R0 = B_popc((~(V^CC))&0x1F7DF7DF),
                            R1 = B_popc((~((V>>1)^CC))&0x1F7DF7DF);
#ifdef RISCV
                        unsigned int Val = (*(unsigned short int *) PtO);
                        R0 += Val&0xFF; R1 += (Val>>8); Val = R0|(R1<<8);
#else
                        unsigned int V1 = R0|(R1<<8);
                        v4s Val = (v4s) (unsigned int) (*(unsigned short int *) PtO);
                        Val = __builtin_pulp_add4(Val, (v4s) V1);
#endif
                        *(unsigned short int *)PtO = (unsigned short int)(unsigned int)Val; PtO+=Wo;
                        V = V>>((2*K+1)+1);
                }
                PtO1+=2;
        }
        PtO1 = Out+Wo*Ho_F+Wo_F+2*(Iter/2);
        for (int w=Wo_F+2*(Iter/2); w<Wo_L; w++) {
                unsigned int V, N;
                PtBit = InBit+(Ho_F*Stride)*W + (w*Stride);
                PtByte = (unsigned char *) (PtBit/8);
                char *PtO = (char *) PtO1;
                V = 0;
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  0); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  5); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  10); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                V = B_ins(V, N, 5,  15); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                for (int h=Ho_F; h<Ho_L; h++) {
                        N = B_extu_r(*(unsigned short int *)PtByte, 5, (PtBit%8));
                        V = B_ins(V, N, 5,  20); PtBit += W; PtByte = (unsigned char *) (PtBit/8);
                        *PtO += B_popc((~(V^C))&0x1FFFFFF); PtO+=Wo;
                        V = V>>(2*K+1);
                }
                PtO1++;
        }
}

void RunTest(int Which, int Iter, int Trace, char *Mode,int * num_ops)

{
	unsigned int Ti;
	ArgConvT Arg;
	ArgConvBT Arg1;
	Ty *IN, *OUT, *FILTER;

	switch (Which) {
		case 0:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) MaxPooling(IN, Wi, Hi, OUT);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
			*num_ops = Ti;
            if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Max Pool", Ti);
			break;
		case 1:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) AvgPooling(IN, Wi, Hi, OUT);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Avg Pool", Ti);
			break;
		case 2:
			IN = Mem; OUT = Mem+Wic*Hic; FILTER = Mem+Wic*Hic+(Wic-4)*(Hic-4); CheckMem(Wic*Hic+(Wic-4)*(Hic-4)+5*5);
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) Additive5x5Convolution(IN, Wic, Hic, FILTER, OUT, 6);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wic-4)*(Hic-4)*Iter, "5x5 Convolutions", Ti);
			break;
		case 3:
			IN = Mem; OUT = Mem+Wil; FILTER = Mem+Wil+Hil; CheckMem(Wil+Hil + Wil*Hil);
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) Linear(IN, Wil, Hil, FILTER, OUT, 6);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, Wil*Hil*Iter, "Linear", Ti);
			break;
		case 4:
			{
				unsigned int In = (unsigned int) Mem;
				signed char *Out =  (signed char *)Mem +((Wxor*Hxor + 7)/8);
				unsigned int Filter = (unsigned int) Mem + ((Wxor*Hxor + 7)/8)*8 + (Wxor-4)*(Hxor-4)*8;
				gap8_resethwtimer();
				WriteGpio(GPIO, 1);
				Ti = gap8_readhwtimer();
				for (int i=0; i<Iter; i++) XnorConv5x5(In, Out, Filter, Wxor, Hxor);
				Ti = gap8_readhwtimer() - Ti;
				WriteGpio(GPIO, 0);
                *num_ops = Ti;
			}
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wxor-4)*(Hxor-4)*Iter, "Xnor Conv 5x5", Ti);
			break;
#ifndef RISCV
		case 5:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) MaxPoolingVect(IN, Wi, Hi, OUT);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Max Pool Vect", Ti);
			break;
		case 6:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) AvgPoolingVect(IN, Wi, Hi, OUT);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Avg Pool Vect", Ti);
			break;
		case 7:
			IN = Mem; OUT = Mem+Wic*Hic; FILTER = Mem+Wic*Hic+(Wic-4)*(Hic-4); CheckMem(Wic*Hic+(Wic-4)*(Hic-4)+5*5);
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) Additive5x5ConvolutionVect(IN, Wic, Hic, FILTER, OUT, 6);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
            if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, (Wic-4)*(Hic-4)*Iter, "5x5 Convolutions Vect", Ti);
			break;
		case 8:
			IN = Mem; OUT = Mem+Wil; FILTER = Mem+Wil+Hil; CheckMem(Wil+Hil + Wil*Hil);
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) LinearVect(IN, Wil, Hil, FILTER, OUT, 6);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles\n", Which, Mode, Wil*Hil*Iter, "LinearVect", Ti);
			break;
		case 9:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			Arg.In=IN; Arg.W=Wi; Arg.H=Hi; Arg.Out=OUT;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(8, (void *) ParMaxPooling, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Parallel Max Pool", Ti, gap8_ncore());
			break;
		case 10:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			Arg.In=IN; Arg.W=Wi; Arg.H=Hi; Arg.Out=OUT;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParAvgPooling, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Parallel Avg Pool", Ti, gap8_ncore());
			break;
		case 11:
			IN = Mem; OUT = Mem+Wic*Hic; FILTER = Mem+Wic*Hic+(Wic-4)*(Hic-4); CheckMem(Wic*Hic+(Wic-4)*(Hic-4)+5*5);
			Arg.In=IN; Arg.W=Wic; Arg.H=Hic; Arg.Filter = FILTER; Arg.Out=OUT; Arg.Norm = 6;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParAdditive5x5Convolution, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
            if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wic-4)*(Hic-4)*Iter, "Parallel 5x5 Convolution", Ti, gap8_ncore());
			break;
		case 12:
			IN = Mem; OUT = Mem+Wil; FILTER = Mem+Wil+Hil; CheckMem(Wil+Hil + Wil*Hil);
			Arg.In=IN; Arg.W=Wil; Arg.H=Hil; Arg.Filter = FILTER; Arg.Out=OUT; Arg.Norm = 6;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParLinear, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, Wil*Hil*Iter, "Parallel Linear", Ti, gap8_ncore());
			break;
		case 13:
			{
				unsigned int In = (unsigned int) Mem;
				signed char *Out =  (signed char *)Mem +((Wxor*Hxor + 7)/8);
				unsigned int Filter = (unsigned int) Mem + ((Wxor*Hxor + 7)/8)*8 + (Wxor-4)*(Hxor-4)*8;
				Arg1.InBit = In; Arg1.Out = Out; Arg1.FilterBit = Filter; Arg1.W = Wxor; Arg1.H = Hxor;
				gap8_resethwtimer();
				WriteGpio(GPIO, 1);
				Ti = gap8_readhwtimer();
				for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParXnorConv5x5, (void *) &Arg1);
				Ti = gap8_readhwtimer() - Ti;
				WriteGpio(GPIO, 0);
                *num_ops = Ti;
			}
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wxor-4)*(Hxor-4)*Iter, "Parallel Xnor Conv 5x5", Ti, gap8_ncore());
			break;
		case 14:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			Arg.In=IN; Arg.W=Wi; Arg.H=Hi; Arg.Out=OUT;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParMaxPoolingVect, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Parallel Max Pool Vect", Ti, gap8_ncore());
			break;
		case 15:
			IN = Mem; OUT = Mem+Wi*Hi; CheckMem(Wi*Hi+(Wi/2)*(Hi/2));
			Arg.In=IN; Arg.W=Wi; Arg.H=Hi; Arg.Out=OUT;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParAvgPoolingVect, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wi/2)*(Hi/2)*Iter, "2x2/2 Parallel Avg Pool Vect", Ti, gap8_ncore());
			break;
		case 16:
			IN = Mem; OUT = Mem+Wic*Hic; FILTER = Mem+Wic*Hic+(Wic-4)*(Hic-4); CheckMem(Wic*Hic+(Wic-4)*(Hic-4)+5*5);
			Arg.In=IN; Arg.W=Wic; Arg.H=Hic; Arg.Filter = FILTER; Arg.Out=OUT; Arg.Norm = 6;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParAdditive5x5ConvolutionVect, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, (Wic-4)*(Hic-4)*Iter, "Parallel Convolution Vect", Ti, gap8_ncore());
			break;
		case 17:
			// IN = Mem; OUT = Mem+Wil; FILTER = Mem+Wil+Hil; CheckMem(Wil+Hil + Wil*Hil);
			IN = Mem; FILTER = Mem+Wil; OUT = Mem+Wil+Wil*Hil; CheckMem(Wil+Hil + Wil*Hil);
			Arg.In=IN; Arg.W=Wil; Arg.H=Hil; Arg.Filter = FILTER; Arg.Out=OUT; Arg.Norm = 6;
			gap8_resethwtimer();
			WriteGpio(GPIO, 1);
			Ti = gap8_readhwtimer();
			for (int i=0; i<Iter; i++) rt_team_fork(gap8_ncore(), (void *) ParLinearVect, (void *) &Arg);
			Ti = gap8_readhwtimer() - Ti;
			WriteGpio(GPIO, 0);
            *num_ops = Ti;
			if (Trace) printf("[%2d][%s] %7d %35s: %8d cycles, %1d Cores\n", Which, Mode, Wil*Hil*Iter, "Parallel Linear Vect", Ti, gap8_ncore());
            break;
#endif
	}
}

int benchmarks(ClusterArg_t * ArgC){

    int   test_num         = ArgC->test_num;
    char* Mode             = ArgC->Mode;
    int   Trace            = ArgC->Trace;
    int   NumIter          = ArgC->Iter;
    
    RunTest(test_num, NumIter, Trace, Mode, &(ArgC->Iter_operations));
        
    return 0;
}


int main()
{
    long start_time, end_time;
    long int tot_time, op_num,kernel_op_num;
    float res,res_kernel;
    int cur_test=0;

    #if !ALIM_1_VOLT
    PMU_set_voltage(1150,0);
    PMU_set_voltage(1200,0);
    #else
    PMU_set_voltage(1000,0);
    #endif

    #ifndef NOGPIO
    rt_padframe_profile_t *profile_gpio = rt_pad_profile_get("hyper_gpio");

    if (profile_gpio == NULL) {
        printf("pad config error\n");
        return 1;
    }
    rt_padframe_set(profile_gpio);
    // GPIO initialization
    rt_gpio_init(0, GPIO);
    rt_gpio_set_dir(0, 1<<GPIO, RT_GPIO_IS_OUT);

    #endif 

    printf("\n\n");
    printf("                      --------------------------------------------------------\n");
    printf("                      --------------------------------------------------------\n");
    printf("                      ---------------   GAP8 benchmarks   --------------------\n");
    printf("                      --------------------------------------------------------\n");
    printf("                      --------------------------------------------------------\n\n\n");


    printf("Gap8 Input Voltage    : %s\n",ALIM_1_VOLT?"1.0 Volt":"1.2 Volts");
    printf("Fabric Controller Freq: %d MhZ\n", FREQ_FC/1000000);
    printf("Cluster  Freq         : %d MhZ\n\n\n", FREQ_CL/1000000);
    
    printf("Number of iterations for each benchmark: %d\n\n\n", ITERATIONS);
    

    if (rt_event_alloc(NULL, 8)) return -1;

    rt_cluster_mount(MOUNT, CID, 0, NULL);
    
    //Set Fabric Controller and Cluster Frequencies
    rt_freq_set(RT_FREQ_DOMAIN_FC, FREQ_FC);
    rt_freq_set(RT_FREQ_DOMAIN_CL, FREQ_CL);
    
    for(int j=0; j < TOT_TEST; j++ ){
        printf("\n                      ---------------   %15s   ---------------\n",tests_titles[j]);
        for(int i=0; i < test_num[j]; i++ ){

            Arg.test_num        = cur_test++;  
            Arg.Iter            = ITERATIONS;
            Arg.Trace           = ENABLE_CYCLE_TRACE;
            #ifdef BYTE
            strcpy(Arg.Mode,"Byte");
            #else
            strcpy(Arg.Mode,"Short");
            #endif

            start_time = rt_time_get_us();
            rt_cluster_call(NULL, CID, (void *) benchmarks, &Arg, NULL, 0, 0, 8, NULL);
            end_time = rt_time_get_us();
            
            tot_time = end_time-start_time;
            op_num   = Arg.Iter_operations;
            
            printf ("%30s Input: %dx%d (x%d iterations) Time: %10ld uSec. Cycles: %10ld\n",tests_names[i], test_input_w[i],test_input_h[i], ITERATIONS, tot_time, op_num);

        }
    }


    rt_cluster_mount(UNMOUNT, CID, 0, NULL);

    return 0;
}