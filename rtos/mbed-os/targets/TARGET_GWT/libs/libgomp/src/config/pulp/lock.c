/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
		Andrea Marongiu - a.marongiu@unibo.it
   info         Lock primitives implementation */

#include "omp-lock.h"

/*********************************** standard APIs ***********************************************/

void omp_set_lock(omp_lock_t *lock) {
    gomp_hal_lock(lock);
}

void omp_unset_lock(omp_lock_t *lock) {
    gomp_hal_unlock(lock);
}

void omp_init_lock(omp_lock_t *lock) {
    gomp_hal_init_lock(lock);
}

int omp_test_lock(omp_lock_t *lock) {
    return gomp_hal_test_lock(lock);
}

void omp_destroy_lock(omp_lock_t *lock) {
    gomp_hal_destroy_lock(lock);
}
