/* Copyright 2014 DEI - Universita' di Bologna
 *   author       DEI - Universita' di Bologna
 *                Davide Rossi         - davide.rossi@unibo.it
 *                Alessandro Capotondi - alessandro.capotondi@unibo.it
 *   info         Master Slave Hardware Barriers implementation. */


#ifndef __BAR_HW_H__
#define __BAR_HW_H__

/* #include "gap_cluster.h" */

/*** *** Low Level Event Unit APIs *** ***/
ALWAYS_INLINE void
gomp_hal_wait_hwBarrier_buff( uint32_t barrier_id )
{
    EU_BarrierTriggerWaitClear();
}

ALWAYS_INLINE void
gomp_hal_clear_hwBarrier_buff( )
{
    EU_BarrierTriggerWaitClear();
}


ALWAYS_INLINE void
gomp_hal_set_hwBarrier( uint32_t barrier_id,
                        uint32_t nthreads,
                        uint32_t thMask )
{
    EU_BarrierSetup(thMask);
}

ALWAYS_INLINE void
gomp_hal_wait_hwEvent_buff( )
{
    EU_EVT_WaitAndClr();
}

/*** *** Master Slave Barrier APIs *** ***/

ALWAYS_INLINE void
MSGBarrier_hwWait( uint32_t barrier_id,
                   uint32_t nthreads,
                   uint32_t thMask)
{
    gomp_hal_wait_hwBarrier_buff( barrier_id );
}

ALWAYS_INLINE void
MSGBarrier_hwSlaveEnter( uint32_t barrier_id)
{
    gomp_hal_wait_hwBarrier_buff(barrier_id);
    gomp_hal_wait_hwEvent_buff( );
}

ALWAYS_INLINE void
MSGBarrier_hwRelease( uint32_t thMask )
{
    gomp_hal_hwTrigg_core( thMask );
}

ALWAYS_INLINE void
gomp_hal_hwBarrier( uint32_t barrier_id)
{
    gomp_hal_wait_hwBarrier_buff( barrier_id );
}

ALWAYS_INLINE void
MSGBarrier_hwDocking ( uint32_t pid )
{
    /* Notify the master I'm on the barrier */
    *(LNFLAGS(pid)) = 0x1U;

#ifdef OMP_BAR_DEBUG
    printf("[%d][%d][MSGBarrier_hwDocking] Arrived %d at 0x%x (%x)\n", get_proc_id(), get_cl_id(), pid, LNFLAGS( pid ), *(LNFLAGS( pid )));
#endif

    gomp_hal_wait_hwEvent_buff( );
}

#endif /*__BAR_HW_H__*/
