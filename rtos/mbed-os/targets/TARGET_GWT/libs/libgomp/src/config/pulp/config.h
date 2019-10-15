/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
		Andrea Marongiu - a.marongiu@unibo.it
   info         Libgomp PULP HAL config */


#ifndef __PULP_OMP_CONFIG_H__
#define __PULP_OMP_CONFIG_H__

#include <gap_cluster.h>

#define NUM_HW_BARRIER          ( 0x04U )

extern char _libgomp_start;

#define LIBGOMP_BASE_ADDR       ((int)&_libgomp_start)

//------------------------------------------------------------
// Builtin command line parameter defaults
//------------------------------------------------------------
#define SIZEOF_UNSIGNED         ( 0x04U )
#define SIZEOF_PTR              ( 0x04U )
#define SIZEOF_INT              ( 0x04U )
#define SIZEOF_WORD             ( 0x04U )
#define LIBGOMP_LBASE           ( LIBGOMP_BASE_ADDR )
#define LIBGOMP_BASE            ( LIBGOMP_BASE_ADDR )
#define CLUSTER_OFFSET          ( 0x00400000 )

/* Maximum number of clusters supported */
#ifndef DEFAULT_MAXCL
#define DEFAULT_MAXCL           ( 0x01U )
#endif

/* Maximum number of PEs per-cluster supported */
#ifndef DEFAULT_MAX_PE
#define DEFAULT_MAX_PE          ( 0x09U )
#endif

/* Maximum number of processors supported */
#ifndef DEFAULT_MAXPROC
#define DEFAULT_MAXPROC         ( DEFAULT_MAX_PE )
#endif

#ifdef __riscv__
#define PCER_ALL_EVENTS_MASK CSR_PCER_ALL_EVENTS_MASK
#define PCMR_ACTIVE CSR_PCMR_ACTIVE
#define PCMR_SATURATE CSR_PCMR_SATURATE
#else
#define PCER_ALL_EVENTS_MASK SPR_PCER_ALL_EVENTS_MASK
#define PCMR_ACTIVE SPR_PCMR_ACTIVE
#define PCMR_SATURATE SPR_PCMR_SATURATE
#endif

//------------------------------------------------------------------------------
// Hardwired Event Unit Offsets
//------------------------------------------------------------------------------

#define OFFSET_EV_BUFF_CLEAR   ( 0x0400U + 0x0004U )
#define OFFSET_CORE_CLKGATE    ( 0x0400U + 0x000CU )

#define OFFSET_TRIGG_BARRIER   ( 0x0800U + 0x035CU )
#define OFFSET_EVENT0          ( 0x0800U + 0X0360U )
#define OFFSET_WAIT_BARRIER    ( 0x0800U + 0x036CU )

#endif
