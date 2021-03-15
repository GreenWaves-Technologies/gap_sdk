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
uint32 exp_17_15(uint32 x);
uint32 pow_17_15(uint32 x, uint32 y);

int square_17_15(int x);
#endif