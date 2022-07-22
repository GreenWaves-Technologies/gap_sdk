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
extern int TileNAddressAndSize(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Address, int *Size, int *Length, MemHierarchy_T Where);
extern int LogicalTileNAddressAndSize(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Address, int *Size, int *Length, MemHierarchy_T Where);
extern void LogicalTileNShape(Kernel_T *Ker, Kernel_Arg_T *Arg, int N, int *Width, int *Height);
extern int LogicalTileMaxWidth(Kernel_T *Ker, Kernel_Arg_T *Arg);
uint64_t SetSpaceSizeOrig(Kernel_T *Ker, Kernel_Arg_T *Arg, uint64_t *ArgSpaceSize);
extern void LogicalTileNAddressAndSizeOrig(Kernel_T *Ker, Kernel_Arg_T *Arg, uint64_t *ArgSpaceSize, int N, uint64_t *Address, uint64_t *Size, uint64_t *Length, uint64_t *Stride);
extern char *BindOpImage(ArgBindingOper Op);

extern char *KernelArgImage(Kernel_T *Ker, Kernel_Arg_T *Arg, CKernel_Arg_T *ArgVal, KernelArgSelect_T ArgSel, KernelIteratorT ArgSpace, KernelIteratorT ItSpace, int *IsInvar);
extern int EvalArgOverflow(Kernel_T *Ker, Kernel_Arg_T *Arg, Object_T *Obj);


#endif
