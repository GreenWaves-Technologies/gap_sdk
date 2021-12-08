#ifndef __PMSIS_BACKEND_NATIVE_TASK_API_H__
#define __PMSIS_BACKEND_NATIVE_TASK_API_H__

#include "string.h"
#include "rt/rt_api.h"

typedef int __os_native_task_t;

static inline void pmsis_mutex_take(pmsis_mutex_t *mutex) {}

static inline void pmsis_mutex_release(pmsis_mutex_t *mutex) {}

static inline int pmsis_mutex_init(pmsis_mutex_t *mutex) {return 0;}

static inline int pmsis_mutex_deinit(pmsis_mutex_t *mutex) {return 0;}

static inline void pmsis_spinlock_init(pmsis_spinlock_t *spinlock) {}

static inline void pmsis_spinlock_take(pmsis_spinlock_t *spinlock) {}

static inline void pmsis_spinlock_release(pmsis_spinlock_t *spinlock) {}

static inline void *pmsis_task_create(void (*entry)(void*),
        void *arg,
        char *name,
        int priority) {return NULL;}

static inline void pmsis_task_suspend(__os_native_task_t *task) {}

#endif
