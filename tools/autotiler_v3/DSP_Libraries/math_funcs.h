/*
 * Copyright 2021 GreenWaves Technologies, SAS
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


#ifndef __MATH_FUNCS_H
#define __MATH_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Gap.h"

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef signed long long int64;
typedef signed int int32;
typedef signed short int16;
typedef signed char int8;

uint32 sqrt_17_15(uint32 x);
uint32 logn_17_15(uint32 x);
uint32 usqrt_17_15(uint32 x);
uint32 ulogn_17_15(uint32 x);
uint32 exp_17_15(uint32 x);
uint32 pow_17_15(uint32 x, uint32 y);

int square_17_15(int x);

int16 fpsin(int16 i);
// Cos(x) = sin(x + pi/2)
#define fpcos(i) fpsin((int16)(((uint16)(i)) + 8192U))

#endif