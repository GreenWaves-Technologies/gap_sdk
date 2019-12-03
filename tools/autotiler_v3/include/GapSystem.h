#ifndef __GAP_SYSTEM_H__
#define __GAP_SYSTEM_H__

#ifndef __EMUL__
#if defined(__PULP_OS__)
#include "rt/rt_api.h"
#endif
#else
#include <stdlib.h>
#endif

#ifdef __EMUL__
typedef void * rt_pointerT;
typedef unsigned int rt_dma_copy_t;
typedef unsigned int rt_hyperram_req_t;
typedef unsigned int rt_hyperram_t;
typedef unsigned int rt_hyperram_conf_t;
#else
typedef unsigned int rt_pointerT;
#endif


/* Memory spaces */
#ifdef __EMUL__
#define L2_MEM
#define L1_CL_MEM
#else
#define L2_MEM		__attribute__((section(".heapl2ram")))
#define L1_CL_MEM 	__attribute__((section(".heapsram")))
#endif

/* Memory Allocation */
#ifdef __EMUL__
#define RT_ALLOC_CL_DATA 0
#define rt_alloc(__where, __size) malloc(__size)
#define gap_allocL1(x) malloc(x)
#define gap_allocL2(x) malloc(x)
#define gap_freeL1(x, y) free((x))
#define gap_freeL2(x, y) free((x))
#else
#if defined(__PULP_OS__)
#define gap_allocL1(x)     rt_alloc( RT_ALLOC_CL_DATA, (x) )
#define gap_allocL2(x)     rt_alloc( RT_ALLOC_L2_CL_DATA, (x) )
#define gap_freeL1(x, y)   rt_free(RT_ALLOC_CL_DATA, (x), (y))
#define gap_freeL2(x, y)   rt_free(RT_ALLOC_L2_CL_DATA, (x), (y))
#else
#define gap_allocL1(x)     pi_l1_malloc( NULL, (x) )
#define gap_allocL2(x)     pi_l2_malloc( (x) )
#define gap_freeL1(x, y)   pi_l1_free(NULL, (x), (y))
#define gap_freeL2(x, y)   pi_l2_free((x), (y))
#endif  /* __PULP_OS__ */
#endif

/* Cluster id, Core Id */
#ifdef __EMUL__

#define gap_coreid()                    0
#define gap_clusterid()                 0
#define gap_ncore()                     1
#define gap_ncorem1()                   1

#else

#define gap_coreid()			__builtin_pulp_CoreId()
#define gap_clusterid()			__builtin_pulp_ClusterId()
#define gap_ncore()			__builtin_pulp_CoreCount()
#if defined (__gap9__)
#define gap_ncorem1()			__builtin_pulp_CoreCount_m1()
#else
#define gap_coreidm1()			__builtin_pulp_CoreId()
#endif

#endif

/* System */
#ifdef __EMUL__

typedef struct{
    void (*cb)(void *);
    void * cb_arg;
} __event_cb;

static __event_cb * event_get(void (*callback)(void *), void *arg){
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

#define rt_team_fork(__cores, __fn, __arg)
#define rt_cluster_call(a,b,__fn,__arg,c,d,e,f,g) Private_call(__fn, __arg, g)
#define rt_event_get(a,b,c) event_get(b,c)
#define rt_event_t __event_cb

#define __CALL(Entry, Arg)	Entry((Arg))

#define gap_setupbarrier(BarN, CoreM)
#define gap_waitbarrier(BarN)

#define rt_event_sched_init(x)
#define rt_event_alloc(x,y) 0
#define rt_nb_pe() 1
#define rt_cluster_mount(a,b,c,d)
#define rt_event_execute(a,b)
#define rt_team_barrier()

static inline void rt_hyperram_conf_init(rt_hyperram_conf_t *conf)
{
}

static inline rt_hyperram_t *rt_hyperram_open(char *dev_name, rt_hyperram_conf_t *conf, rt_event_t *event)
{
}

static inline void rt_hyperram_close(rt_hyperram_t *handle, rt_event_t *event)
{
}

/* Definitions for PMSIS API when emulating. */
struct pi_cluster_task
{
    void *entry;
    void *arg;
    unsigned int stack_size;
};
#define pi_open_from_conf(a,b)
#define pi_cluster_open(a)               (0) /* This function returns a value. */
#define pi_cluster_send_task_to_cl(a, b) Private_call(((struct pi_cluster_task *) b)->entry, ((struct pi_cluster_task *) b)->arg, NULL)
#define pi_cluster_close(a)
#define pi_l2_malloc(x)                  malloc((void *) (x))
#define pi_l2_free(x, y)                 free((void *) (x))
#else

//All this stuff should not stay here!!!!!!!!!!!!!!!

#if defined(PMSIS_DRIVERS)
#include "pmsis.h"

extern struct pi_device *hyperram;

#define __cl_hyper_copy(a,b,c,d,e,f)                                    \
    pi_cl_hyper_copy((struct pi_device *) a, (uint32_t) b, (void *) c, d, f, (pi_cl_hyper_req_t *) e)

#define __cl_hyper_copy_2d(a,b,c,d,e,f,g,h)                             \
    pi_cl_hyper_copy_2d((struct pi_device *) a, (uint32_t) b, (void *) c, d, e, f, h, (pi_cl_hyper_req_t *) g)

#define __cl_hyper_copy_wait        pi_cl_hyper_read_wait

#define CL_HYPER_EXT2LOC 1
#define CL_HYPER_LOC2EXT 0

static inline void __cl_dma_memcpy(uint32_t ext, uint32_t loc, uint16_t size, pi_cl_dma_dir_e dir, uint8_t merge, pi_cl_dma_copy_t *copy)
{
    copy->dir = dir;
    copy->merge = merge;
    copy->size = size;
    copy->id = 0;
    copy->loc = loc;
    copy->ext = ext;
    copy->stride = 0;
    copy->length = 0;
    pi_cl_dma_memcpy(copy);
}


static inline void __cl_dma_memcpy_2d(uint32_t ext, uint32_t loc, uint16_t size, uint16_t stride, uint16_t length, pi_cl_dma_dir_e dir, uint8_t merge, pi_cl_dma_copy_t *copy)
{
    copy->dir = dir;
    copy->merge = merge;
    copy->size = size;
    copy->id = 0;
    copy->loc = loc;
    copy->ext = ext;
    copy->stride = stride;
    copy->length = length;
    pi_cl_dma_memcpy_2d(copy);
}

#endif

//////////////////////////////////////////////

#define __CALL(Entry, Arg)

// #define rt_alloc(Space, Size)		malloc((Size))
// #define rt_alloc_align(Space, Size)	malloc((Size))
// #define rt_free(Space, Pointer, Size)	free((Pointer))

#if defined(PMSIS_DRIVERS)
//This is used anywhere, better leave undefined
//#define gap_setupbarrier(BarN, CoreM)
#define gap_setupbarrier(BarN, CoreM)
#define gap_waitbarrier(BarN)           pi_cl_team_barrier()

#elif defined(__PULP_OS__)
//This is used anywhere, better leave undefined
//#define gap_setupbarrier(BarN, CoreM)   eu_bar_setup(eu_bar_addr(BarN), CoreM)
#define gap_setupbarrier(BarN, CoreM)   eu_bar_setup(eu_bar_addr(BarN), CoreM)
#define gap_waitbarrier(BarN)           eu_bar_trig_wait_clr(eu_bar_addr(BarN))

#endif

#endif


/* Memcopy */
#ifdef __EMUL__

typedef struct {
    void (*callback)(void *);
} rt_event_sched_t;


#define RT_DMA_DIR_LOC2EXT 0
#define RT_DMA_DIR_EXT2LOC 1

/* Internal counter for debug purpose, track the amount of transfered data to/from L2/L3 */
unsigned int __L3_Read, __L3_Write, __L2_Read, __L2_Write;

static int Private_gap_dma_memcpy(rt_pointerT ext, rt_pointerT loc, unsigned short size, int ext2loc, int L3)

{
        int i;
        char *To   = (ext2loc==RT_DMA_DIR_EXT2LOC)?((char *) loc):((char *) ext);
        char *From = (ext2loc==RT_DMA_DIR_EXT2LOC)?((char *) ext):((char *) loc);

	if (ext2loc==RT_DMA_DIR_EXT2LOC) {
		if (L3) __L3_Read += size; else __L2_Read += size;
	} else {
		if (L3) __L3_Write += size; else __L2_Write += size;
	}

        for (i=0; i<size; i++) To[i] = From[i];
        return 1; 
}

static int Private_gap_dma_memcpy_2d(rt_pointerT ext, rt_pointerT loc, unsigned short size, unsigned int stride, unsigned short length, int ext2loc, int L3)

{
        int CopyIn = (ext2loc==RT_DMA_DIR_EXT2LOC);
        char *To   = CopyIn?((char *) loc):((char *) ext);
        char *From = CopyIn?((char *) ext):((char *) loc);
        int i, j, Chunk;

	if (ext2loc==RT_DMA_DIR_EXT2LOC) {
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

#define rt_dma_memcpy(ExtAddr, LocAddr, Size, Dir, Merge, Descr) Private_gap_dma_memcpy((ExtAddr), (LocAddr), (Size), (Dir), 0)
#define rt_dma_memcpy_2d(ExtAddr, LocAddr, Size, Stride, Length, Dir, Merge, Descr) Private_gap_dma_memcpy_2d((ExtAddr), (LocAddr), (Size), (Stride), (Length), (Dir), 0)
#define rt_dma_wait(Descr)

#define __rt_hyperram_cluster_copy(Dev, ExtAddr, LocAddr, Size, Descr, Dir) Private_gap_dma_memcpy((ExtAddr), (LocAddr), (Size), (Dir), 1)
#define __rt_hyperram_cluster_copy_2d(Dev, ExtAddr, LocAddr, Size, Stride, Length, Descr, Dir) Private_gap_dma_memcpy_2d((ExtAddr), (LocAddr), (Size), (Stride), (Length), (Dir), 1)
#define rt_hyperram_cluster_wait(Descr)
#endif


#endif
