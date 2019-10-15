/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         gomp mutex implementation */

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "omp-lock.h"

#define gomp_mutex_lock(x)      omp_set_lock((unsigned int *)(x))
#define gomp_mutex_unlock(x)    omp_unset_lock((unsigned int *)(x))

#endif // _MUTEX_H__
