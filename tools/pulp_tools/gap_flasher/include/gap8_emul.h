#ifndef __GAP8_EMUL_H__
#define __GAP8_EMUL_H__

typedef   signed short v2s __attribute__((vector_size (4)));
typedef unsigned short v2u __attribute__((vector_size (4)));

typedef   signed char  v4s __attribute__((vector_size (4)));
typedef unsigned char  v4u __attribute__((vector_size (4)));

typedef void * rt_pointerT;

#define DMA_COPY_IN 1
#define DMA_COPY_OUT 0

#define L2_MEM
#define L1_CL_MEM
#define L1_FC_MEM

#define RT_L1_DATA
#define RT_L1_GLOBAL_DATA
#define RT_L1_BSS
#define RT_L1_ALIAS_DATA
#define RT_L1_TINY_DATA
#define RT_FC_TINY_DATA
#define RT_FC_GLOBAL_DATA
#define RT_FC_SHARED_DATA
#define RT_L2_DATA
#define RT_L2_RET_DATA

#define RT_FC_DATA RT_FC_GLOBAL_DATA

/* Packing of scalars into vectors */
#define gap8_pack2(x, y)		((v2s) {(signed short)   (x), (signed short)   (y)})
#define gap8_packu2(x, y)		((v2u) {(unsigned short) (x), (unsigned short) (y)})

#define gap8_pack4(x, y, z, t)		((v4s) {(signed char)   (x), (signed char)   (y), (signed char)   (z), (signed char)   (t)})
#define gap8_packu4(x, y, z, t)		((v4u) {(unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t)})

/* Max */
#define gap8_max2(x, y) 		((v2s) {((signed short)(x)[0]>(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
						((signed short)(x)[1]>(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap8_max4(x, y) 		((v4s) {((signed char)(x)[0]>(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
						((signed char)(x)[1]>(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
						((signed char)(x)[2]>(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
						((signed char)(x)[3]>(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap8_maxu2(x, y) 		((v2u) {((unsigned short)(x)[0]>(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
						((unsigned short)(x)[1]>(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap8_maxu4(x, y) 		((v4u) {((unsigned char)(x)[0]>(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
						((unsigned char)(x)[1]>(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
						((unsigned char)(x)[2]>(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
						((unsigned char)(x)[3]>(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Min */
#define gap8_min2(x, y) 		((v2s) {((signed short)(x)[0]<(signed short)(y)[0])?((signed short)(x)[0]):((signed short)(y)[0]), \
						((signed short)(x)[1]<(signed short)(y)[1])?((signed short)(x)[1]):((signed short)(y)[1])})
#define gap8_min4(x, y) 		((v4s) {((signed char)(x)[0]<(signed char)(y)[0])?(signed char)(x)[0]:(signed char)(y)[0], \
						((signed char)(x)[1]<(signed char)(y)[1])?(signed char)(x)[1]:(signed char)(y)[1], \
						((signed char)(x)[2]<(signed char)(y)[2])?(signed char)(x)[2]:(signed char)(y)[2], \
						((signed char)(x)[3]<(signed char)(y)[3])?(signed char)(x)[3]:(signed char)(y)[3]})

#define gap8_minu2(x, y) 		((v2u) {((unsigned short)(x)[0]<(unsigned short)(y)[0])?(unsigned short)(x)[0]:(unsigned short)(y)[0], \
						((unsigned short)(x)[1]<(unsigned short)(y)[1])?(unsigned short)(x)[1]:(unsigned short)(y)[1]})
#define gap8_minu4(x, y) 		((v4u) {((unsigned char)(x)[0]<(unsigned char)(y)[0])?(unsigned char)(x)[0]:(unsigned char)(y)[0], \
						((unsigned char)(x)[1]<(unsigned char)(y)[1])?(unsigned char)(x)[1]:(unsigned char)(y)[1], \
						((unsigned char)(x)[2]<(unsigned char)(y)[2])?(unsigned char)(x)[2]:(unsigned char)(y)[2], \
						((unsigned char)(x)[3]<(unsigned char)(y)[3])?(unsigned char)(x)[3]:(unsigned char)(y)[3]})

/* Clip */
#define gap8_clip(x, precision)		((x)<(-(1<<(precision)))?(-(1<<(precision))):(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x)))
#define gap8_clipu(x, precision)	((x)<0)?0:(((x)>((1<<(precision))-1))?((1<<(precision))-1):(x))

/* Abs */
#define gap8_abs2(x) 			((v2s) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1]})
#define gap8_abs4(x) 			((v4s) {((x)[0]<0)?-(x)[0]:(x)[0], ((x)[1]<0)?-(x)[1]:(x)[1], \
						((x)[2]<0)?-(x)[2]:(x)[2], ((x)[3]<0)?-(x)[3]:(x)[3]})

/* Mac */
#define	gap8_macs(Acc, x, y)		((Acc) + ((short int) (x) * (short int) (y)))
#define	gap8_machhs(Acc, x, y)		((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16)))
#define	gap8_macu(Acc, x, y)		((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))
#define	gap8_machhu(Acc, x, y)		((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))

#define	gap8_macsN(Acc, x, y, n)	(((Acc) + ((short int) (x) * (short int) (y)))>>(n))
#define	gap8_macuN(Acc, x, y, n)	(((Acc) + ((unsigned short int) (x) * (unsigned short int) (y)))>>(n))
#define	gap8_macsRN(Acc, x, y, n)	((((Acc) + ((short int) (x) * (short int) (y))) + (1<<((n)-1))) >> (n))
#define	gap8_macuRN(Acc, x, y, n)	((((Acc) + ((unsigned short int) (x) * (unsigned short int) (y))) + (1<<((n)-1))) >> (n))

#define	gap8_machhsN(Acc, x, y, n)	(((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) >> (n))
#define	gap8_machhuN(Acc, x, y, n)	(((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) >> (n))
#define	gap8_machhsRN(Acc, x, y, n)	((((Acc) + ((short int) ((x)>>16) * (short int) ((y)>>16))) + (1<<((n)-1))) >> (n))
#define	gap8_machhuRN(Acc, x, y, n)	((((Acc) + ((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16))) + (n)))

/* Multiplications */
#define gap8_muls(x, y)			(((short int) (x) * (short int) (y)))
#define gap8_mulu(x, y)			(((unsigned short int) (x) * (unsigned short int) (y)))
#define gap8_mulhhs(x, y)		(((short int) ((x)>>16) * (short int) ((y)>>16)))
#define gap8_mulhhu(x, y)		(((unsigned short int) ((x)>>16) * (unsigned short int) ((y)>>16)))
#define gap8_mulsN(x, y, n)		(((short int) (x) * (short int) (y))>>(n))
#define gap8_mulsRN(x, y, n)		((((short int) (x) * (short int) (y)) + (1<<((n)-1)))>>(n))
#define gap8_muluN(x, y, n)		(((unsigned short int) (x) * (unsigned short int) (y))>>(n))
#define gap8_muluRN(x, y, n)		((((unsigned short int) (x) * (unsigned short int) (y)) + (1<<((n)-1)))>>(n))

/* Vectorial product and sum of products */
#define gap8_dotp2(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap8_dotpu2(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap8_dotpus2(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1])

#define gap8_sumdotp2(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap8_sumdotpu2(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])
#define gap8_sumdotpus2(x, y, z)	((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1])

#define gap8_dotp4(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap8_dotpu4(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap8_dotpus4(x, y)		(    (x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])

#define gap8_sumdotp4(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap8_sumdotpu4(x, y, z)		((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])
#define gap8_sumdotpus4(x, y, z)	((z)+(x)[0]*(y)[0] + (x)[1]*(y)[1] + (x)[2]*(y)[2] + (x)[3]*(y)[3])


/* Complex Multiplication, Q15x15 into Q15, with optional post scaling by 1 or 2 */
#define gap8_cplxmuls(x, y)		((v2s) {(signed short) ((((long long) (x)[0]*(long long) (y)[0]) - ((long long) (x)[1]*(long long) (y)[1]))>>15),  \
						(signed short) ((((long long) (x)[0]*(long long) (y)[1]) + ((long long) (x)[1]*(long long) (y)[0]))>>15)})
#define gap8_cplxmulsdiv2(x, y)		(gap8_cplxmuls(x, y)>>(v2s){1,1})
#define gap8_cplxmulsdiv4(x, y)		(gap8_cplxmuls(x, y)>>(v2s){2,2})

/* Complex conjugate */
#define gap8_cplxconj(x)		((v2s) {(x)[0], -(x)[1]})

/* Complex rotation by -pi/2 */
#define gap8_cplxmj(x)			((v2s) {(x)[1], -(x)[0]})

/* Complex substration, result rotated by -pi/2 */
#define gap8_sub2rotmj(x, y)		((v2s) {(x)[1]-(y)[1], (y)[0]-(x)[0]})

/* Complex addition with post scaling by 1 or 2 */
#define gap8_add2div2(x, y)		(((x)+(y))>>(v2s) {1, 1})
#define gap8_add2div4(x, y)		(((x)+(y))>>(v2s) {2, 2})

/* Complex subtraction with post scaling by 1 or 2 */
#define gap8_sub2div2(x, y)		(((x)-(y))>>(v2s) {1, 1})
#define gap8_sub2div4(x, y)		(((x)-(y))>>(v2s) {2, 2})

/* Viterbi Max and Viterbi Select, pair of Q15 */
#define gap8_vitmax(x, y) 		(_VitT1_Flag=((x)[1]<=(y)[1])?1:0, _VitT0_Flag=((x)[0]<=(y)[0])?1:0,\
				 	(v2s) {((x)[0]>(y)[0])?(x)[0]:(y)[0], ((x)[1]>(y)[1])?(x)[1]:(y)[1]})
#define gap8_vitsel(x, y) 		(v2s) {(_VitT0_Flag?(y)[0]:(x)[0])<<1|_VitT0_Flag, (_VitT1_Flag?(y)[1]:(x)[1])<<1|_VitT1_Flag}

/* Position of the most significant bit of x */
#define gap8_fl1(x)			(31 - __builtin_clz((x)))

/* Number of sign bits */
#define gap8_clb(x)			(__builtin_clrsb((x)))

/* Bit Extraction */
#define gap8_bitextract(x, size, off) 		(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap8_bitextractu(x, size, off)		(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap8_bitextract_r(x, size, off) 	(((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap8_bitextractu_r(x, size, off)	(((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap8_bitextract_r_safe(x, size, off) 	(((((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))<<(32-((((size)>32)?32:(size)))))>>(32-((((size)>32)?32:(size)))))
#define gap8_bitextractu_r_safe(x, size, off)	(((x)>>((off)&0x1F))&((unsigned int)(1<<((((size)>32)?32:(size))))-1))

/* Bit insertion */
#define gap8_bitinsert(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap8_bitinsert_r(dst, src, size, off) 	(((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))
#define gap8_bitinsert_r_safe(dst, src, size, off) 	(((dst) & ~(((1<<(((size)>32)?32:(size)))-1)<<((off)&0x1F))) | (((src) & ((1<<(((size)>32)?32:(size)))-1))<<((off)&0x1F)))

/* 1 bit rotation to the right, 32 bits input */
#define gap8_rotr(x)			((((x)>>1)&0x7FFFFFFF) | ((x)<<31))

/* Add with normalization and rounding */
#define gap8_addroundnormu(x, y, scale)		((unsigned int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnormu_reg(x, y, scale)	((unsigned int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnorm(x, y, scale)		((int)((x) + (y) + (1<<((scale)-1)))>>(scale))
#define gap8_addroundnorm_reg(x, y, scale)	((int)((x) + (y) + (1<<((scale)-1)))>>(scale))

/* Normalization and rounding */
#define gap8_roundnormu(x, scale)		((unsigned int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnormu_reg(x, scale)		((unsigned int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnorm(x, scale)		((int)((x) + (1<<((scale)-1)))>>(scale))
#define gap8_roundnorm_reg(x, scale)		((int)((x) + (1<<((scale)-1)))>>(scale))



/* For non Gap targets, compiled with -D__EMUL__ */
/* START_EMUL_BTIN */
#define gap8_coreid()					0
#define gap8_clusterid()				0
#define gap8_ncore()					1

#define gap8_sprwrite(x, y)
#define gap8_sprread(x)					((int) 0)
#define gap8_sprread_vol(x)				((int) 0)

#define gap8_read_base_off(base, off)  	         	((int) 0)
#define gap8_write_base_off(base, off, val)

#define gap8_read_base_off_vol(base, off)       	((int) 0)
#define gap8_read_base_off_half_vol(base, off)  	((int) 0)
#define gap8_read_base_off_byte_vol(base, off) 	 	((int) 0)

#define gap8_write_base_off_half_vol(x, base, off)
#define gap8_write_base_off_byte_vol(x, base, off)

#define rt_core_id  gap8_coreid
/* STOP_EMUL_BTIN */

/* Emulation mode for PC execution or single core simulation run.
   Here we overload the runtime primitives in order to end up with something
   that can compile on a workstation
*/
#include <stdlib.h>
#include <string.h>

#define plp_irq_enable()
#define plp_irq_disable()

#define eu_bar_addr(x)
#define eu_bar_setup(x, m)
#define eu_bar_trig_wait_clr(x)
#define eoc_fetch_enable(x)
#define plp_cluster_wait(x)
#define plp_dma_wait(x)
#define rt_event_sched_init(x)
#define rt_event_alloc(x,y) 0
#define rt_nb_pe() 1
#define rt_cluster_mount(a,b,c,d)
#define rt_event_execute(a,b)
#define rt_team_barrier()

#define plp_timer_conf_low(a, b, c, d, e, f, g, h, i)
#define plp_timer_get_count_low() 			((int) 0)

#define plp_fc_timer_conf_low(a, b, c, d, e, f, g, h, i)
#define plp_fc_timer_get_count_low() 			((int) 0)

#define gap8_sprwrite(x, y)
#define gap8_sprread(x)			((int) 0)
#define gap8_sprread_vol(x)		((int) 0)

#define gap8_read_base_off(base, off)           ((int) 0)
#define gap8_read_base_off_vol(base, off)       ((int) 0)
#define gap8_write_base_off(base, off, val)
#define gap8_write_base_off_vol(base, off, val)

#define plp_setSlaveEntry(coreMask, handler, arg)

#ifndef RT_DMA_DIR_LOC2EXT
#define RT_DMA_DIR_LOC2EXT DMA_COPY_OUT
#endif
#ifndef RT_DMA_DIR_EXT2LOC
#define RT_DMA_DIR_EXT2LOC DMA_COPY_IN
#endif

typedef int rt_dma_copy_t;
typedef struct {
    void (*callback)(void *);
} rt_event_sched_t;

unsigned int __L3_Read, __L3_Write, __L2_Read, __L2_Write;

static int Private_gap8_dma_memcpy(rt_pointerT ext, rt_pointerT loc, unsigned short size, int ext2loc, int L3)

{
        int i;
        char *To   = (ext2loc==DMA_COPY_IN)?((char *) loc):((char *) ext);
        char *From = (ext2loc==DMA_COPY_IN)?((char *) ext):((char *) loc);

	if (ext2loc==DMA_COPY_IN) {
		if (L3) __L3_Read += size; else __L2_Read += size;
	} else {
		if (L3) __L3_Write += size; else __L2_Write += size;
	}

        for (i=0; i<size; i++) To[i] = From[i];
        return 1;
}

static int Private_gap8_dma_memcpy_2d(rt_pointerT ext, rt_pointerT loc, unsigned short size, unsigned int stride, unsigned short length, int ext2loc, int L3)

{
        int CopyIn = (ext2loc==DMA_COPY_IN);
        char *To   = CopyIn?((char *) loc):((char *) ext);
        char *From = CopyIn?((char *) ext):((char *) loc);
        int i, j, Chunk;

	if (ext2loc==DMA_COPY_IN) {
		if (L3) __L3_Read += size; else __L2_Read += size;
	} else {
		if (L3) __L3_Write += size; else __L2_Write += size;
	}
	for (Chunk=0; Chunk<size; Chunk+=length)  {
                for (i=0; i<length; i++) To[i] = From[i];
                if (CopyIn) {
			From += stride; To += length;
		} else {
			To += stride; From += length;
		}
        }
        return 1;
}

#define plp_dma_memcpy(ext, loc, size, ext2loc)	Private_gap8_dma_memcpy((ext), (loc), (size), (ext2loc), 0)
#define plp_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc)	Private_gap8_dma_memcpy_2d((ext), (loc), (size), (stride), (length), (ext2loc), 0)
#define gap8_dma_memcpy(ext, loc, size, ext2loc, merge, copy) Private_gap8_dma_memcpy((ext), (loc), (size), (ext2loc), 0)
#define gap8_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc, merge, copy) Private_gap8_dma_memcpy_2d((ext), (loc), (size), (stride), (length), (ext2loc), 0)
#define gap8_dma_wait(copy)
#define gap8_dma_type int
#define rt_dma_memcpy(ext, loc, size, ext2loc, merge, eventid) plp_dma_memcpy(ext, loc, size, ext2loc)
#define rt_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc, merge, eventid) plp_dma_memcpy_2d(ext, loc, size, stride, length, ext2loc)
#define rt_dma_wait
#define gap8_cluster_udma_memcpy(chan, ext, loc, size, ext2loc)	Private_gap8_dma_memcpy((ext), (loc), (size), (ext2loc), 1)
#define gap8_cluster_udma_memcpy_2d(chan, ext, loc, size, stride, length, ext2loc)	Private_gap8_dma_memcpy_2d((ext), (loc), (size), (stride), (length), (ext2loc), 1)
#define gap8_cluster_udma_wait(chan)	((int) 0)

#define rt_alloc(__where, __size) malloc(__size)

typedef struct{
    void (*cb)(void *);
    void * cb_arg;
} __event_cb;

static __event_cb* event_get(void (*callback)(void *), void *arg){
    __event_cb * event = (__event_cb*) malloc (sizeof(__event_cb));
    event->cb = callback;
    event->cb_arg = arg;
    return event;
}

static int Private_call(void (*fn)(void *), void * arg, __event_cb * event)
{
    fn(arg);
    if(event) event->cb(event->cb_arg);
    return 1;
}

#define rt_team_fork(__cores, __fn, __arg) Private_call(__fn, __arg, 0)
#define rt_cluster_call(a,b,__fn,__arg,c,d,e,f,g) Private_call(__fn, __arg, g)
#define rt_event_get(a,b,c) event_get(b,c)
#define rt_event_t __event_cb


#define START_TIMER()
#define STOP_TIMER()

/* Interrupts Enable/Dsiable */
#define gap8_enable_its()		plp_irq_enable()
#define gap8_disable_its()		plp_irq_disable()

/* HW timer */
#define gap8_resethwtimer()		plp_fc_timer_conf_low(1, 1, 0, 0, 0, 0, 0, 0, 0)
#define gap8_readhwtimer()		plp_fc_timer_get_count_low()

/* Core Counters */
#define gap8_setmodehwcounters(Val)	gap8_sprwrite(HW_COUNT_MODE_COUNTERS, (Val))
#define gap8_setallhwcounters(Val)	gap8_sprwrite((HW_COUNT_BASE_OFFSET+HW_COUNT_WRITE_ALL), (Val))
#define gap8_enablehwcounters(Val)	gap8_sprwrite(HW_COUNT_EN_COUNTERS, (Val))
#define gap8_readhwcounter(Which)	gap8_sprread_vol(HW_COUNT_BASE_OFFSET+(Which))

/* HW barriers */
#define gap8_setupbarrier(BarN, CoreM)	eu_bar_setup(eu_bar_addr(BarN), CoreM)
#define gap8_waitbarrier(BarN)		eu_bar_trig_wait_clr(eu_bar_addr(BarN))

/* Fetch enable */
#define gap8_fetchenable(CoreMask)	eoc_fetch_enable(CoreMask)


/* Utilities */

#define FIX2FP(Val, Precision)		((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)		((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)		((int)((Val)*((1 << (Precision))-1)))



#endif
