/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
   Alessandro Capotondi - alessandro.capotondi@unibo.it
   Germain Haugou - haugoug@iis.ee.ethz.ch
   Andrea Marongiu - a.marongiu@unibo.it
   info         Appsupport for PULP */

#ifndef __APPSUPPORT_H__
#define __APPSUPPORT_H__

#include <stdint.h>
#include <gap_cluster.h>

static inline uint32_t
get_proc_id( )
{
    return __core_ID();
}

static inline uint32_t
get_cl_id( )
{
    return __cluster_ID();
}

static inline uint32_t
get_num_procs()
{
    return __core_NUMBERS();
}

static inline uint32_t
get_num_clusters()
{
    return __cluster_NUMBERS();
}

/*************************************************************
 * Print functions *
 *************************************************************/

#define _printdecp(a, b) printf("%s %d - Processor %d\n", a, b, get_proc_id())
#define _printdect(a, b) printf("%s %d - Time %d\n", a, b, get_time())
#define _printdecn(a, b) printf("%s %d\n", a, b)

#define _printhexp(a, b) printf("%s %x - Processor %d\n", a, b, get_proc_id())
#define _printhext(a, b) printf("%s %x - Time %d\n", a, b, get_time())
#define _printhexn(a, b) printf("%s %x\n", a, b)

#define _printstrp(a) printf("%s - Processor %d\n", a, get_proc_id())
#define _printstrt(a) printf("%s - Time %d\n", a, get_time())
#define _printstrn(a) printf("%s\n", a)

#endif // __APPSUPPORT_H__
