/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __GRAPH_PROCESS_H__
#define __GRAPH_PROCESS_H__
#include "AutoTilerLibTypes.h"
#include "AutoTilerLib.h"

extern CKernel_Arg_T *CNNGraphArgLookup(CNNGraph_T *Graph, NameT *Name, int *Pos);
extern CKernel_Arg_T *CNNGraphLocalLookup(CNNGraph_T *Graph, NameT *Name, int *Pos);
extern GraphEdgeWeb_T *EquivGroupRep(GraphEdgeWeb_T *E);
extern int IsEquivGroupRep(GraphEdgeWeb_T *E);
extern void ExtractGraphStatistics(CNNGraph_T *Graph, int *TotalSize, int *TotalL3Move, int *TotalL2Move,
				   long long int *TotalBaselineBandwidth, long long int *TotalOperation);
extern void MarkKernelsUsedInGraph(CNNGraph_T *Graph);


extern unsigned int DynamicL2_Base;

extern GraphControl_T GraphControl;
extern void AT_DefaultSetGraphCtrl();
extern void GenerateGraphNodeTypeTemplate(CNNGraph_T *Graph, FILE *Fi);
extern void GenerateOneNodeArgInit(Kernel_T *Ker, KernelGroup_T *Group, char *FunArgName, NodeTypeTemplate_T *NodeType, FILE *Fi);

extern void CollectConstructorLoadEvents(CNNGraph_T *Graph, char *LoadEvents);

extern char *GraphNodeArgImage(GraphNode_T *Node, CKernel_Arg_T *Arg, char *Str);


#endif
