/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __GEN_TILING_DUMP_H__
#define __GEN_TILING_DUMP_H__
#include "AutoTilerLibTypes.h"
#include <SDL2/SDL.h>


#define	DBG_DUMP_OBJECT		(1<<0)
#define	DBG_DUMP_KER_ARGS	(1<<1)

extern unsigned int DebugMode;

extern void SetDebugModeBits(unsigned int Mask);
extern void ClrDebugModeBits(unsigned int Mask);
extern char *InlineModeImage(InlineModeT Mode);
extern char *DimImage(KernelIteratorT Dim);
extern char *CallLocationImage(KernelCallLocationT Location);
extern char *TileOrientImage(Tile_Orientation_T Orientation);
extern char *ObjTypeImage(Object_T *O, unsigned int BuffCount);
extern char *ArgTypeImage(Kernel_Arg_T *O);
extern void DumpObjects(Kernel_T *Ker, char *Message, Object_T **ObjList, unsigned int NObj);
extern void KernelMemoryStatistics(Object_T **ObjList, unsigned int NObj, unsigned int Iter,
				   unsigned int *MoveIn, unsigned int *MoveOut,
				   unsigned int *TotalL2Mem, unsigned int *TotalL2MemInOut);
extern void DumpKernelArgs(Kernel_T *Ker, NameT *KernelName, Tile_Orientation_T Orientation,
		    	   Kernel_Arg_T **KerList, Object_T **ObjList, unsigned int Narg,
		    	   unsigned int L1Memory, unsigned int L1TopMemory);

extern char *IterSpaceName(KernelIteratorT Space);
extern char *KerArgName(Kernel_Arg_T *Arg);
extern char *EdgeTypeImage(GraghNodeArgT Type);

extern void DumpGraphNodes(CNNGraph_T *Graph);
extern void DumGraphEdges(GraphEdgeWeb_T *GraphEdges);
extern void DumpOrderedGraphNodes(CNNGraph_T *Graph);
extern void DumpStackedTensors(CNNGraph_T *Graph);
extern void DumpAvailableMemory(char *Mess, char *Name, MemChunk_T **Mem1, MemChunk_T **Mem2, int From, int To);

typedef struct ABox_T Box_T;
typedef struct ABox_T {
	void *Symbol;
        BoxType_T Type;
        char *Name;
        int X;
        int Y;
        int W;
        int H;
        SDL_Surface *Message;
        Box_T *Next;
} Box_T;
extern char *BoxTypeImage(BoxType_T Type);
extern void AddBox(void *Symbol, BoxType_T Type, char *Name, int X, int Y, int W, int H, int Loc);
extern void RemoveBox(void *Symbol, BoxType_T Type, char *Name, int X, int Y, int W, int H, int Loc, int MustExist);
extern void AddOneNodeL1DisplayInfos(GraphNode_T *Node);
extern int DisplayAlloc(CNNGraph_T *Graph);
extern char *CNN_KernelOperImage(KernelOper_T Op);
extern  void DumpAllocInfo(SymbolAlloc_T *Alloc);

extern char *ArgLocImage(AT_MemLocation_T Loc);
extern char *ATMemImage(AT_MemLocation_T Loc);
extern char *MemoryBaseImage(AT_MemLocation_T Mem);
extern void DumpFreeMemSlots(char *Mess, AT_MemLocation_T Mem, MemChunk_T **MemSlots, int Slots);

extern void DumpDynamicMemoryUsage(CNNGraph_T *Graph, char *Banner);

extern void DumAllNodeArgs(CNNGraph_T *Graph);
extern int AT_NeedL2BufferForDump(CNNGraph_T *Graph, unsigned int Filter);
extern int *AT_PrepareNodeGraphDump(CNNGraph_T *Graph, unsigned int Filter, int *NeedBuffer);
extern void AT_DumpGraphNodeArg(CNNGraph_T *Graph, GraphNode_T *Node,
                         unsigned int Filter, int *Collect, char **EventHandle,
                         char *L2_Buffer, unsigned int L2_BufferSize,
                         FILE *Fi);



#endif
