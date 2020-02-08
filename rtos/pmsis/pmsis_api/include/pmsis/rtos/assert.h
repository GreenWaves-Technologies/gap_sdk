/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __PMSIS_ASSERT__H__
#define __PMSIS_ASSERT__H__

#include "stdio.h"
#include "pmsis/rtos/os_frontend_api/os.h"

#define pi_assert(test)                                                 \
    if (!(test))                                                        \
    {                                                                   \
        printf("PMSIS assertion error in %s, %s:%d :\n",__func__, __FILE__, (unsigned int)__LINE__); \
        printf("%s\n", #test);                                          \
        pmsis_exit(-1);                                                 \
    }

#endif
