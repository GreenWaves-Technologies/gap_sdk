/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         PULP OpemMP HAL */

#ifndef __OMP_HAL_H__
#define __OMP_HAL_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <gap_cluster.h>

/* HEADERS */
#include "config.h"

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE static inline __attribute__ ((always_inline))
#endif

#ifndef NULL
#define NULL ((void *) 0x0) /* Standard C */
#endif


#include "appsupport.h"
#include "omp-bar.h"
#include "memutils.h"
#include "omp-lock.h"
#include "mutex.h"

#define gomp_assert(x) \
{\
    if( ! (x)) {\
        printf("[GOMP] Assert failed at file %s line %d\n",__FILE__, __LINE__); \
        abort();\
    }\
}

ALWAYS_INLINE void
perfInitAndStart()
{
#ifdef PROFILE0
    __PCER_Set(0xFFFFFFFF);
    __PCCR31_Set(0);
    __PCMR_Set(PCMR_GLBEN_Msk | PCMR_SATURATE_Msk);
#endif
}

ALWAYS_INLINE void
gomp_hal_init()
{
    // In case one cluster above the maximum number supported enters here,just make it sleep forerever
    if (get_cl_id() >= DEFAULT_MAXCL) {
        while (1) EU_EVT_MaskWaitAndClr(0);
    }

    gomp_assert(get_num_procs() <= DEFAULT_MAX_PE);

    EU_EVT_Clr( 1 << 0 );
    /* Set Event Line to 1 */
    EU_EVT_MaskSet( 1 << 0 );

    /* Start Performance Counters */
    perfInitAndStart();
}



#endif /* __HAL_H__ */
