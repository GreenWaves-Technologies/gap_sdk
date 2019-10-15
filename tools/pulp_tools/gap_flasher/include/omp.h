/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OMP_H__
#define __OMP_H__

#include "pulp.h"
#include <rt/rt_api.h>

typedef unsigned int omp_lock_t;

int omp_get_thread_num(void);

typedef int               kmp_int32;
typedef void (*kmpc_micro)              ( kmp_int32 * global_tid, kmp_int32 * bound_tid, void *args);

typedef struct ident {
    kmp_int32 reserved_1;   /**<  might be used in Fortran; see above  */
    kmp_int32 flags;        /**<  also f.flags; KMP_IDENT_xxx flags; KMP_IDENT_KMPC identifies this union member  */
    kmp_int32 reserved_2;   /**<  not really used in Fortran any more; see above */
    kmp_int32 reserved_3;   /**< source[4] in Fortran, do not use for C++  */
    char const *psource;    /**< String describing the source location.
                            The string is composed of semi-colon separated fields which describe the source file,
                            the function and a pair of line numbers that delimit the construct.
                             */
} ident_t;

typedef kmp_int32 kmp_critical_name[8];

#ifdef OLD_KMPC_FORK_CALL
void __kmpc_fork_call(ident_t *loc, kmp_int32 argc, kmpc_micro microtask, ...);
#else
void __kmpc_fork_call(ident_t *loc, kmp_int32 argc, kmpc_micro microtask, int **args);
#endif

#endif
