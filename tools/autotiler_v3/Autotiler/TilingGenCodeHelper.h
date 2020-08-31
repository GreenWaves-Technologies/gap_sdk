/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef _TILINGGENCODEHELPER_H_
#define _TILINGGENCODEHELPER_H_

#include "AutoTilerLibTypes.h"

extern int IsInKernelIterator(Kernel_T *Ker, KernelIteratorT Iterator);
extern KernelIteratorT ArgIterationSpace(Kernel_T *Ker, Kernel_Arg_T *Arg);

#endif
