/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __TILING_GEN_CODE_H__
#define __TILING_GEN_CODE_H__
#include "AutoTilerLibTypes.h"
#include "AutoTilerLib.h"

extern void SetupRuntimeNames();
extern char *StringAppend3(char *Str1, char *Str2, char *Str3);
extern int IsParametricSpace(Kernel_T *Ker, KernelIteratorT IterSpace);
extern int Use2DCopyReord(Kernel_Arg_T *Arg, int L3toL2);
extern int Use2DCopy(Kernel_Arg_T *Arg, int L3toL2);
extern void TileNAddressAndSize(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Address, int *Size, int *Length);
extern void LogicalTileNAddressAndSize(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Address, int *Size, int *Length);
extern void LogicalTileNShape(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Width, int *Height);
extern int LogicalTileMaxWidth(Kernel_T *Ker, Kernel_Arg_T *Arg);
extern char *BindOpImage(ArgBindingOper Op);


#endif
