/* Copyright 2014 DEI - Universita' di Bologna
 *   author       DEI - Universita' di Bologna
 *                Davide Rossi         - davide.rossi@unibo.it
 *                Alessandro Capotondi - alessandro.capotondi@unibo.it
 *   info         Master Slave Software Barriers implementation. */


#ifndef __BAR_SW_H__
#define __BAR_SW_H__

/* #include "gap_cluster.h" */

/* Global RFLAGS base address */
ALWAYS_INLINE uint32_t
RFLAGS_BASE( uint32_t cid )
{
    uint32_t ret;
    switch( cid )
    {
        case 0x0U:
            ret = SWBAR_BASE_ADDR + ( CLUSTER_OFFSET * 0x0U );
            break;
        case 0x1U:
            ret = SWBAR_BASE_ADDR + ( CLUSTER_OFFSET * 0x1U );
            break;
        case 0x2U:
            ret = SWBAR_BASE_ADDR + ( CLUSTER_OFFSET * 0x2U );
            break;
        case 0x3U:
            ret = SWBAR_BASE_ADDR + ( CLUSTER_OFFSET * 0x3U );
            break;
        default:
            ret = SWBAR_BASE_ADDR + ( CLUSTER_OFFSET * 0x0U );
            break;
    }
    return ret;
}

/* Global NFLAGS base address */
ALWAYS_INLINE uint32_t
NFLAGS_BASE( uint32_t cid )
{
    uint32_t ret;
    switch( cid )
    {
        case 0x0U:
            ret = SWBAR_BASE_ADDR + SWBAR_RFLAGS_SIZE + ( CLUSTER_OFFSET * 0x0U );
            break;
        case 0x1U:
            ret = SWBAR_BASE_ADDR + SWBAR_RFLAGS_SIZE + ( CLUSTER_OFFSET * 0x1U );
            break;
        case 0x2U:
            ret = SWBAR_BASE_ADDR + SWBAR_RFLAGS_SIZE + ( CLUSTER_OFFSET * 0x2U );
            break;
        case 0x3U:
            ret = SWBAR_BASE_ADDR + SWBAR_RFLAGS_SIZE + ( CLUSTER_OFFSET * 0x3U );
            break;
        default:
            ret = SWBAR_BASE_ADDR + SWBAR_RFLAGS_SIZE + ( CLUSTER_OFFSET * 0x0U );
            break;
    }
    return ret;
}

/* Per-Core FLAG OFFSET */
#define FLAG_OFFSET( x )            ( x << Log2SizeofMSGBarrier )

/* Aliased Local Software Wait Barrier */
#define LRFLAGS( pid )              ((volatile MSGBarrier *) ((uint32_t) SWBAR_LBASE_ADDR + FLAG_OFFSET( pid & 0x07U )))
/* Aliased Local Software Notification Barrier */
#define LNFLAGS( pid )              ((volatile MSGBarrier *) ((uint32_t) SWBAR_LBASE_ADDR + SWBAR_RFLAGS_SIZE + FLAG_OFFSET( pid )))

/* Software Wait Barrier */
#define RFLAGS( pid )               ((volatile MSGBarrier *) ( RFLAGS_BASE( pid >> 0x03U ) + FLAG_OFFSET( pid & 0x07U ) ))
/* Software Notification Barrier */
#define NFLAGS( mpid, pid )         ((volatile MSGBarrier *) ( NFLAGS_BASE( mpid >> 0x03U ) + FLAG_OFFSET( pid ) ))

/*** *** Master Slave Barrier APIs *** ***/
ALWAYS_INLINE void
MSGBarrier_Wait( uint32_t nthreads,
                 uint32_t * restrict slave_ids )
{
    uint32_t i;
    for( i = 1; i < nthreads; ++i )
    {
        uint32_t slave_id = slave_ids[i];

#ifdef OMP_BAR_DEBUG
        printf("[%d][%d][MSGBarrier_Wait] Waiting %d at 0x%x (%x)\n", get_proc_id(), get_cl_id(), slave_id, LNFLAGS( slave_id ), *(LNFLAGS( slave_id )));
#endif
        while( *(LNFLAGS( slave_id )) != 0x1U )
            continue;

        /* Reset flag */
        *(LNFLAGS( slave_id )) = 0x0U;

#ifdef OMP_BAR_DEBUG
        printf("[%d][%d][MSGBarrier_Wait] Arrived %d\n", get_proc_id(), get_cl_id(), slave_id);
#endif
    }
}

ALWAYS_INLINE void
MSGBarrier_swDocking_Wait( uint32_t nteams)
{
    for( uint32_t i = 1; i < nteams; ++i )
    {
#ifdef OMP_BAR_DEBUG
        printf("[%d][%d][MSGBarrier_swDocking_Wait] Waiting %d at 0x%x (%x)\n", get_proc_id(), get_cl_id(), i, NFLAGS( i, 0x0U ), *(NFLAGS( i, 0x0U )));
#endif
        while( *(LNFLAGS( i )) != 0x1U )
            continue;

        /* Reset flag */
        *(LNFLAGS( i )) = 0x0U;

#ifdef OMP_BAR_DEBUG
        printf("[%d][%d][MSGBarrier_Wait] Arrived %d\n", get_proc_id(), get_cl_id(), i);
#endif
    }
}

ALWAYS_INLINE void
MSGBarrier_swDocking ( uint32_t pid )
{
#ifdef OMP_BAR_DEBUG
    printf("[%d][%d][MSGBarrier_swDocking] Arrived %d at 0x%x (%x)\n", get_proc_id(), get_cl_id(), pid, LNFLAGS( pid ), *(LNFLAGS( pid )));
#endif

    volatile MSGBarrier *rflag = LRFLAGS( pid );
    /* Read start value */
    volatile MSGBarrier  old_val = *rflag;

    /*Notify the master I'm on the barrier */
    *(NFLAGS( 0x0U, get_cl_id() )) = 0x1U;

    while(1)
    {
        volatile MSGBarrier *curr_val = LRFLAGS( pid );
        if (old_val == *curr_val)
            continue;
        break;
    }
}

// ALWAYS_INLINE void
// MSGBarrier_SlaveEnter ( uint32_t pid )
// {
//     volatile MSGBarrier *rflag = LRFLAGS( pid );
//     /* Read start value */
//     volatile MSGBarrier  old_val = *rflag;
//      Notify the master I'm on the barrier
//     *(LNFLAGS( pid )) = 0x1U;
//     while(1)
//     {
//         volatile MSGBarrier *curr_val = LRFLAGS( pid );
//         if (old_val == *curr_val)
//             continue;
//         break;
//     }
// }

// ALWAYS_INLINE void
// MSGBarrier_Release( uint32_t nthreads,
//                     uint32_t * restrict slave_ids )
// {
//     uint32_t i;
//     for( i = 1; i < nthreads; ++i )
//     {
//         uint32_t slave_id = slave_ids[i];
//         volatile MSGBarrier *rflag = LRFLAGS( slave_id );
//         (*rflag)++;
//     }
// }

// ALWAYS_INLINE void
// gomp_hal_barrier( uint32_t pid,
//                   uint32_t mpid,
//                   uint32_t nthreads,
//                   uint32_t * restrict slave_ids )
// {
//     if( pid == mpid )
//     {
//         MSGBarrier_Wait( nthreads, slave_ids);
//         MSGBarrier_Release( nthreads, slave_ids);
//     }
//     else
//     {
//         MSGBarrier_SlaveEnter( mpid, pid );
//     }
// }
#endif /*__BAR_SW_H__*/
