/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __GEN_TILING_H__
#define __GEN_TILING_H__

#include "HashName.h"

/** @defgroup AutoTilerTypes AutoTilerDataTypes
@ingroup groupAutoTiler
@{ */

/**
@brief Defined CNN Kernel operations

Defined CNN Kernel operations
*/
typedef enum {
        KOP_NONE                        = 0,

	/* Primitive operations */
	KOP_SETBIAS,
        KOP_CONV,
        KOP_CONV_DP,
        KOP_CONV_DW,
        KOP_CONV_DWDP,
	KOP_DP_REDUCT,
	KOP_DP_REDUCT_IO,
	KOP_DP_REDUCT_MULBIAS,
	KOP_DP_REDUCT_IO_MULBIAS,
	KOP_DP_REDUCT_MULBIAS_SCALAR,
	KOP_DP_REDUCT_IO_MULBIAS_SCALAR,
        KOP_MAXPOOL,
        KOP_AVGPOOL,
        KOP_GLOBAL_POOL_INIT,
        KOP_GLOBAL_MAXPOOL,
        KOP_GLOBAL_AVGPOOL,
        KOP_GLOBAL_POOL_FINAL,
        KOP_RELU,
	KOP_RELUN,
	KOP_HSIGMOID,
	KOP_HSWISH,
	KOP_LEAKYRELU,
        KOP_LINEAR,
        KOP_LINEAR_DP,
	KOP_DP_REDUCT_LINEAR,
        KOP_MATADD,
        KOP_MATADD_DYNADJUST,
        KOP_MATMUL,
        KOP_MATMUL_SCALE,
        KOP_MATMUL_SCALE_SCALAR,
	KOP_MATSCALE,
	KOP_MATSCALE_SCALAR,
        KOP_SOFTMAX,

	/* Grouped operations */
        KOP_CONV_RELU,
        KOP_CONV_RELUN,
        KOP_CONV_MAXPOOL,
        KOP_CONV_MAXPOOL_RELU,
        KOP_CONV_MAXPOOL_RELUN,
        KOP_CONV_AVGPOOL,
        KOP_CONV_AVGPOOL_RELU,
        KOP_CONV_AVGPOOL_RELUN,

        KOP_CONV_DP_RELU,
        KOP_CONV_DP_RELUN,
        KOP_CONV_DP_MAXPOOL,
        KOP_CONV_DP_MAXPOOL_RELU,
        KOP_CONV_DP_MAXPOOL_RELUN,
        KOP_CONV_DP_AVGPOOL,
        KOP_CONV_DP_AVGPOOL_RELU,
        KOP_CONV_DP_AVGPOOL_RELUN,

        KOP_CONV_DW_RELU,
        KOP_CONV_DW_RELUN,
        KOP_CONV_DW_MAXPOOL,
        KOP_CONV_DW_MAXPOOL_RELU,
        KOP_CONV_DW_MAXPOOL_RELUN,
        KOP_CONV_DW_AVGPOOL,
        KOP_CONV_DW_AVGPOOL_RELU,
        KOP_CONV_DW_AVGPOOL_RELUN,

        KOP_CONV_DWDP_RELU,
        KOP_CONV_DWDP_RELUN,
        KOP_CONV_DWDP_MAXPOOL,
        KOP_CONV_DWDP_MAXPOOL_RELU,
        KOP_CONV_DWDP_MAXPOOL_RELUN,
        KOP_CONV_DWDP_AVGPOOL,
        KOP_CONV_DWDP_AVGPOOL_RELU,
        KOP_CONV_DWDP_AVGPOOL_RELUN,

        KOP_MAXPOOL_RELU,
        KOP_MAXPOOL_RELUN,
        KOP_AVGPOOL_RELU,
        KOP_AVGPOOL_RELUN,
        KOP_LINEAR_RELU,
        KOP_LINEAR_RELUN,
        KOP_MATADD_RELU,
        KOP_MATADD_RELUN,
        KOP_MATMUL_RELU,
        KOP_MATMUL_RELUN,

	KOP_LAST

} KernelOper_T;

typedef enum {
        KACT_NONE = 0,
        KACT_RELU,
        KACT_RELUN,
        KACT_HSIGMOID,
        KACT_HSWISH,
        KACT_LEAKY,
} CNN_ActivationOper_T;

typedef enum {
	PAD_LEFT, 		/* All padding elements are inserted on the left/top */
	PAD_RIGHT, 		/* All padding elements are inserted on the right/bottom */
	PAD_BALANCED_LEFT,	/* Padding is balanced as [Pad-Pad/2, Pad/2] */
	PAD_BALANCED_RIGHT	/* Padding is balanced as [Pad/2, Pad-Pad/2] */
} AT_PadType;

/* Visible to tiler API */

typedef   signed char  v4s __attribute__((vector_size (4)));

typedef enum {
        K_INP = 0,
        K_OUTP = 1,
        K_DIM_LAST = 3,
} KernelDimT;

typedef enum {
	KER_DIM0=0,
	KER_DIM1=1,
	KER_DIM2=2,
	KER_DIM3=3,
	KER_DIM4=4,
} KernelIterationDimT;

/**
@brief Iterators definition

Iterators definition
*/
typedef enum {
	KER_ITER_TILE0=0,	/**< Most inner iteration space operating on the basic data plane of a user kernel argument */
	KER_ITER_TILE1=1,	/**< Second inner iteration space operating on the basic data plane of a user kernel argument */
	KER_ITER_TILE2=2,	/**< Third inner iteration space operating on the basic data plane of a user kernel argument */
	KER_ITER_D0=3,
	KER_ITER_D1=4,
	KER_ITER_D2=5,
	KER_ITER_D3=6,
	KER_ITER_LAST=7		/**< Marker for last */
} KernelIteratorT;
/// @cond PrivateStuff
#define KER_ITER_TILE_MAX 3
#define IS_TILED_SPACE(Space) ((Space)<KER_ITER_TILE_MAX)
#define IterSpaceToTile(Space) (((Space)==0)?O_TILE0:(((Space)==1)?O_TILE1:(((Space)==2)?O_TILE2:0)))
#define TileToIterSpace(Tile) (((Tile)==O_TILE0)?KER_ITER_TILE0:(((Tile)==O_TILE1)?KER_ITER_TILE1:(((Tile)==O_TILE2)?KER_ITER_TILE2:0)))

#define CG_MAX_PIPE_DEPTH	2


#define KER_TILE		0
#define KER_TILE1		1
#define KER_TILE2		2
#define KER_IN_PLANE		3
#define KER_OUT_PLANE		4
#define KER_IN_OUT_PLANE	5

/// @endcond

/**
@brief Basic kernel call type

Basic kernel call type

A basic kernel can be called as a sequential call from master core in the cluster, in this case only master core executes the code and arguments are passed with the usual C type list of parameters.

It can also be called on all the available cores in the clusters, this is a parallel call. In this case arguments are grouped into a single structure which address is passed to the parallel dispatcher.
*/
typedef enum {
	CALL_SEQUENTIAL=0,	/**< Call the related basic kernel only on master core */
	CALL_PARALLEL=1,	/**< Call the related basic kernel on all available cores */
	CALL_SEQUENTIAL_STRUCT=2,/**< Call the related basic kernel only on master core but pass all arguments through one structure */
} KernelCallTypeT;

/**
@brief Call location attribute
*/
typedef enum {
	CALL_IN=0,		/**< Call in the iterator body */
	CALL_PROLOG=1,		/**< Call before the iterator starts */
	CALL_EPILOG=2,		/**< Call after the iterator stops */
	CALL_LAST=3,
} CallLocationT;

/**
@brief Call location for basic kernels in the user kernel defined iteration space

Call location for basic kernels in the user kernel defined iteration space

When the context is a group of user kernels called kernels are user kernels and not basic kernels and in this case only LOC_IN_GROUP is valid
*/
typedef enum {
	LOC_LOOP=0,		/**< Call basic kernel in the loop body of iterator 0 on the basic data planes */
	LOC_LOOP_PROLOG=1,	/**< Call basic kernel right before the first iteration of iterator 0 on the basic data planes */
	LOC_LOOP_EPILOG=2,	/**< Call basic kernel right after the last iteration of iterator 0 on the basic data planes */

	LOC_LOOP1=3,		/**< Call basic kernel in the loop body of iterator 1 on the basic data planes  */
	LOC_LOOP1_PROLOG=4,	/**< Call basic kernel right before the first iteration of iterator 2 on the basic data planes */
	LOC_LOOP1_EPILOG=5,	/**< Call basic kernel right after the last iteration of iterator 2 on the basic data planes */

	LOC_LOOP2=6,		/**< Call basic kernel in the loop body of iterator 2 on the basic data planes */
	LOC_LOOP2_PROLOG=7,	/**< Call basic kernel right before the first iteration of iterator 2 on the basic data planes */
	LOC_LOOP2_EPILOG=8,	/**< Call basic kernel right after the last iteration of iterator 2 on the basic data planes */

	LOC_D0=9,		/**< Call basic kernel inside Iter D0 body */
	LOC_D0_PROLOG=10,	/**< Call basic kernel before 1st iteration on D0 */
	LOC_D0_EPILOG=11,	/**< Call basic kernel after last iteration on D0 */

	LOC_D1=12,		/**< Call basic kernel inside Iter D1 body */
	LOC_D1_PROLOG=13,	/**< Call basic kernel before 1st iteration on D1 */
	LOC_D1_EPILOG=14,	/**< Call basic kernel after last iteration on D1 */

	LOC_D2=15,		/**< Call basic kernel inside Iter D2 body */
	LOC_D2_PROLOG=16,	/**< Call basic kernel before 1st iteration on D2 */
	LOC_D2_EPILOG=17,	/**< Call basic kernel after last iteration on D2 */

	LOC_D3=18,		/**< Call basic kernel inside Iter D3 body */
	LOC_D3_PROLOG=19,	/**< Call basic kernel before 1st iteration on D3 */
	LOC_D3_EPILOG=20,	/**< Call basic kernel after last iteration on D3 */

	LOC_PROLOG=21,			/**< Call basic kernel before the entire user kernel iteration structure */
	LOC_EPILOG=22,			/**< Call basic kernel after the entire user kernel iteration structure */

	LOC_GROUP=23,			/**< Call user kernel in a kernel group */
} KernelCallLocationT;

extern KernelCallLocationT IterCallLocation[][CALL_LAST];

#define LOC_INNER_LOOP			0
#define LOC_INNER_LOOP_PROLOG		1
#define LOC_INNER_LOOP_EPILOG		2

#define LOC_INNER_LOOP1			3
#define LOC_INNER_LOOP1_PROLOG		4
#define LOC_INNER_LOOP1_EPILOG		5

#define LOC_INNER_LOOP2			6
#define LOC_INNER_LOOP2_PROLOG		7
#define LOC_INNER_LOOP2_EPILOG		8

#define LOC_IN_PLANE			9
#define LOC_IN_PLANE_PROLOG		10
#define LOC_IN_PLANE_EPILOG		11

#define LOC_OUT_PLANE			12
#define LOC_OUT_PLANE_PROLOG		13
#define LOC_OUT_PLANE_EPILOG		14

#define LOC_IN_OUT_PLANE		15
#define LOC_IN_OUT_PLANE_PROLOG		16
#define LOC_IN_OUT_PLANE_EPILOG		17

/**
@brief User kernel tiling orientation

User kernel tiling orientation

Tiling orientation is global to the user kernel applying to every single user kernel argument unless a given user kernel argument has its own orientation
defined in it's set of constraints
*/
typedef enum {
	TILE_HOR,	/**< Tiles are horizontal, full width and variable height */
	TILE_VER,	/**< Tiles are vertical, full height and variable width */
	TILE_UNDEF	/**< Undefined orientation */
} Tile_Orientation_T;

/**
@brief User kernel argument constraints

User kernel argument constraints
*/
typedef enum {
	OBJ_CONSTRAINTS_NONE = 0,		/**< No constraints on this user kernel argument */
	OBJ_CONSTRAINTS_EVEN = (1<<1),		/**< Variable tile size generated for this user kernel should be even */
	OBJ_CONSTRAINTS_ODD = (1<<2),		/**< Variable tile size generated for this user kernel should be odd */
	OBJ_CONSTRAINTS_ONEPREFTILE = (1<<3),	/**< This user kernel argument has a prefered tile variable size, use only a single tile for it */
	OBJ_CONSTRAINTS_TILE_HOR = (1<<4),	/**< Force this kernel argument to be tiled horizontaly */
	OBJ_CONSTRAINTS_TILE_VER = (1<<5),	/**< Force this kernel argument to be tiled verticaly */
	OBJ_CONSTRAINTS_PAD_REM = (1<<6),	/**< When argument has non integer dim ratio use last tile to recover missing elements if possible */
	OBJ_CONSTRAINTS_DROP_REM = (1<<7),	/**< When argument has non integer dim ratio simply drop them */
	OBJ_CONSTRAINTS_DYNAMIC = (1<<8),	/**< When argument has non integer dim ratio dynamically evaluate tile size using DimRatio */
	OBJ_CONSTRAINTS_2D = (1<<9),		/**< Argument is 2D strided */
	OBJ_CONSTRAINTS_3D = (1<<10),
} KernelArgConstraints_T;

/**
@brief Tile attributes used in argument bindings

Tile attributes used in argument bindings
*/
typedef enum {
	KER_ARG_NONE = 0,		/**< No attributes */
	KER_ARG_TILE = 1,		/**< The current tile base address in shared L1 */
	KER_ARG_TILE_NEXTDB = 2,	/**< The next tile base address in shared L1, in case argument is double buffered this is the next double buffer */
	KER_ARG_TILE_OFF = 3,		/**< The current tile byte offset relative to user kernel shared L1 allocated memory */
	KER_ARG_TILE_1_2 = 4,		/**< The current tile base address in shared L1 plus half of the tile size */
	KER_ARG_TILE_W = 5,		/**< Current tile width */
	KER_ARG_TILE_USEDW = 6,		/**< Current tile used width */
	KER_ARG_TILE_H = 7,		/**< Current tile height */
	KER_ARG_TILE_USEDH = 8,		/**< Current tile used height */
	KER_ARG_TILE_W0 = 9,		/**< Tile standard width (not the one for the last iteration that is potentially smaller) */
	KER_ARG_TILE_H0 = 10,		/**< Tile standard height (not the one for the last iteration that is potentially smaller) */
	KER_ARG = 11,			/**< User kernel argument base */
	KER_ARG_W = 12,			/**< User kernel argument width */
	KER_ARG_H = 13,			/**< User kernel argument height */
	KER_ARG_NTILES = 14,		/**< Number of tiles for related user kernel argument */
	KER_ARG_TILEINDEX = 15,		/**< Current tile index for related user kernel argument, starts at 0 */
	KER_ARG_TILE_BASE = 16,		/**< Current tile base in line or column unit, when argument is dynamic it is computed at runtime */
	KER_ARG_IT_INDEX = 17,		/**< Actual value of iterator attached to ItSpace */
	KER_ARG_PAD = 18,		/**< Actual padding of a feature space associated to arg (left,right,top,bottom) as a v4s */
	KER_ARG_TILE_PAD = 19,		/**< Actual padding of tile associated to arg (left,right,top,bottom) as a v4s */
	KER_ARG_PARTILE_DIM = 20,	/**< Actual dimension of a parametric space */
	KER_ARG_PARTILE_SIZE = 21,	/**< Size of a tile from a parametric space */
	KER_ARG_LOADEDPARTILE_SIZE = 22,/**< Size of a tile from a parametric space, in case the related subspace has been promoted to partial buffer returns the dimension of this subspace otherwise is equal to KER_ARG_PARTILE_SIZE */
	KER_IT_INDEX = 23,		/**< Actual value of a given kernel iterator */

	TC_ARG = 24,			/**< A C argument */
	TC_IMM = 25,			/**< An immediate int value */
	TC_USYMB = 26,			/**< A user defined symbol */
	TC_KDIM = 27,			/**< One of the user Kernel Dimensions */
	TC_ARG_IND = 28,		/**< An indirection on a C argument */
	TC_ARG_IND_IT_INDEX = 29, 	/**< An indirection on a C argument with respect to actual value of ItSpace */
	TC_ARG_PLUS_IT_INDEX = 30, 	/**< A C argument added to actual value of ItSpace, ItSpace multiplied by a constant */


	/* Deprecated */
	KER_ARG_INPLANEINDEX = 40,	/**< Current Input Plane index for related user kernel argument, starts at 0 */
	KER_ARG_OUTPLANEINDEX = 41,	/**< Current Output Plane index for related user kernel argument, starts at 0 */

	TC_ARG_IND_IP = 42,		/**< A C argument subscripted by the current In Data Plane iteration index */ // SHOULD BE REPLACED BY TC_ARG_IND_DIM Carg[Dim_Index]
	TC_ARG_IND_OP = 43,		/**< A C argument subscripted by the current Out Data Plane iteration index */
	TC_ARG_IND_TILE = 44,		/**< A C argument subscripted by the current current most inner iteration index on argument basic data plane */
	TC_ARG_PLUS_IND_IP = 45,	/**< A C argument added to the current In Data Plane iteration index multiplied by a constant */
	TC_ARG_PLUS_IND_OP = 46,	/**< A C argument added to the current Out Data Plane iteration index multiplied by a constant*/
	TC_ARG_PLUS_IND_TILE = 47,	/**< A C argument added to the current current most inner iteration index on argument basic data plane multiplied by a constant */

	TC_ARG_PLUS_OFFSET = 48,	/**< A C argument plus an immediate offset */
	TC_ARG_PLUS_C_OFFSET = 49,	/**< A C argument plus a C variable name offset */
} KernelArgSelect_T;

/// @cond PrivateStuff
#define	OBJ_IS(Obj, Prop)	(((Obj)->Type & (Prop)) == (Prop))
#define	OBJ_SET(Obj, Prop)	((Obj)->Type = ((Obj)->Type | (Prop)))
#define	OBJ_CLR(Obj, Prop)	((Obj)->Type = ((Obj)->Type & ~(Prop)))
/// @endcond

/**
@brief Elementary properties of a user kernel argument

Elementary properties of a user kernel argument

A user kernel argument object type is a logical or of these elementary properties. In general properties come into pair: a property and not a property.
Predefined group of properties in enum Object_Type_T provide a set of consistent set of elementary properties.
*/

typedef enum {
        O_IN                    = (1<<0),       /**< Argument is an input */
        O_NIN                   = (1<<1),       /**< Argument is not an input */
        O_OUT                   = (1<<2),       /**< Argument is an output */
        O_NOUT                  = (1<<3),       /**< Argument is not an output */
        O_BUFF                  = (1<<4),       /**< Argument is a buffer */
        O_NBUFF                 = (1<<5),       /**< Argument is not a buffer */
        O_TILED                 = (1<<6),       /**< Argument should be tiled */
        O_NTILED                = (1<<7),       /**< Argument should not be tiled */
        O_ONETILE               = (1<<8),       /**< Argument should be exactly one tile in shared L1 */
        O_NONETILE              = (1<<9),       /**< Argument should can have multiple tiles in shared L1 */
        O_DB                    = (1<<10),      /**< Argument should be double buffered (or triple) */
        O_NDB                   = (1<<11),      /**< Argument should be single buffered */
        O_L2DB                  = (1<<12),      /**< Argument is in L3 and should be double or triple buffered in L2 */
        O_NL2DB                 = (1<<13),      /**< Argument is not in L3 */
        O_ALIAS                 = (1<<14),      /**< Argument is aliased to another argument, different name but same memory location */
        O_NALIAS                = (1<<15),      /**< Argument is not aliased */
        O_DYNTILE               = (1<<16),      /**< Argument tile size is adjusted dynamically */
        O_NDYNTILE              = (1<<17),      /**< Argument tile size is not adjusted dynamically */
	O_CONST			= (1<<18),	/**< Argument is constant, applies to input only */
	O_NCONST		= (1<<19),	/**< Argument is not constant, applies to input only */

        O_TILE2                 = (1<<29),      /**< Argument traverses the 3rd level of iteration on the basic data plane */
        O_TILE1                 = (1<<30),      /**< Argument traverses the 2nd level of iteration on the basic data plane */
        O_TILE0                 = (1<<31),      /**< Argument traverses the 1st level of iteration on the basic data plane, this should always be the case if tiled */
} BasicObjectType_T;

/// @cond PrivateStuff
#define O_TILEMASK              (O_TILE0|O_TILE1|O_TILE2)
#define OBJ_ITER(Obj)           ((Obj)->Type & (O_TILEMASK))
/// @endcond

/**
@brief Pre defined user kernel argument types

Pre defined user kernel argument types
*/
typedef enum {
	OBJ_IN 				= O_IN|O_TILED,
	OBJ_IN_DB 			= O_IN|O_DB|O_TILED,
	OBJ_IN_L2DB 			= O_IN|O_L2DB|O_TILED,
	OBJ_IN_DB_L2DB 			= O_IN|O_DB|O_L2DB|O_TILED,

	OBJ_OUT 			= O_OUT|O_TILED,
	OBJ_OUT_DB 			= O_OUT|O_DB|O_TILED,
	OBJ_OUT_L2DB 			= O_OUT|O_L2DB|O_TILED,
	OBJ_OUT_DB_L2DB 		= O_OUT|O_DB|O_L2DB|O_TILED,

	OBJ_OUT_DB_ALIAS 		= O_OUT|O_DB|O_ALIAS|O_TILED,

	OBJ_IN_OUT 			= O_IN|O_OUT|O_TILED,
	OBJ_IN_OUT_DB 			= O_IN|O_OUT|O_DB|O_TILED,
	OBJ_IN_OUT_L2DB 		= O_IN|O_OUT|O_L2DB|O_TILED,
	OBJ_IN_OUT_DB_L2DB 		= O_IN|O_OUT|O_DB|O_L2DB|O_TILED,

	OBJ_BUFFER			= O_BUFF|O_TILED,
	OBJ_BUFFER_NTILED		= O_BUFF,

	OBJ_BUFFER_IN			= O_BUFF|O_IN|O_TILED,
	OBJ_BUFFER_IN_NTILED		= O_BUFF|O_IN,

	OBJ_BUFFER_OUT			= O_BUFF|O_OUT|O_TILED,
	OBJ_BUFFER_OUT_NTILED		= O_BUFF|O_OUT,

	OBJ_BUFFER_IN_OUT		= O_BUFF|O_IN|O_OUT|O_TILED,
	OBJ_BUFFER_IN_OUT_NTILED	= O_BUFF|O_IN|O_OUT,

	OBJ_BUFFER_IN_L2DB		= O_BUFF|O_IN|O_L2DB|O_TILED,

	OBJ_BUFFER_IN_L2DB_NTILED	= O_BUFF|O_IN|O_L2DB,

	OBJ_BUFFER_OUT_L2DB		= O_BUFF|O_OUT|O_L2DB|O_TILED,

	OBJ_BUFFER_OUT_L2DB_NTILED	= O_BUFF|O_OUT|O_L2DB,

	OBJ_BUFFER_IN_OUT_L2DB		= O_BUFF|O_IN|O_OUT|O_L2DB|O_TILED,

	OBJ_BUFFER_IN_OUT_L2DB_NTILED	= O_BUFF|O_IN|O_OUT|O_L2DB,

	OBJ_BUFFER_ONETILE		= O_BUFF|O_TILED|O_ONETILE,
	OBJ_BUFFER_DYNTILE		= O_BUFF|O_TILED|O_DYNTILE,
	OBJ_BUFFER_DB_ONETILE		= O_BUFF|O_DB|O_TILED|O_ONETILE,

} Object_Type_T;

/**
@brief Control AutoTiler inline strategy

Control AutoTiler inline strategy
*/
typedef enum {
	NEVER_INLINE=0,	/**< Never inline user kernels */
	ALWAYS_INLINE=1,	/**< Always inline user kernels */
	SINGLE_INLINE=2, /**< Inline a user kernel when it appears only once */
} InlineModeT;

/**
@brief User Kernel optimization

brief User Kernel optimization
*/
typedef enum {
	KER_OPT_NONE  = 0, /**< No optimization */
	KER_OPT_BUFFER_PROMOTE = (1<<0), /**< When all user kernel arguments can fit into given L1 memory promote them to buffer */
	KER_OPT_PARTIAL_BUFFER_PROMOTE = (1<<1), /**< When all tile of a user kernel argument across In Planes can fit into given L1 memory promote them to partial buffer */
} KernelOptimizationT;

/**
@brief Argument binding selection

Argument binding selection
*/
typedef enum {
	BIND_K_ARG = 0,	/**< Binds to a user kernel argument */
	BIND_C_ARG = 1,	/**< Binds to a C user kernel argument */
	BIND_IMM = 2,	/**< Binds to an immediate value */
	BIND_USYMB = 3,	/**< Binds to a user defined symbol */
	BIND_KDIM = 4,	/**< Binds to one of the user kernel dimension */
} ArgBindingT;

/**
@brief Argument binding selection, operation with a constant

Argument binding selection, operation with a constant
*/
typedef enum {
	BIND_OP_NOP=1,
	BIND_OP_PLUS=2,
	BIND_OP_MINUS=3,
	BIND_OP_MULT=4,
	BIND_OP_DIV=5,
	BIND_OP_MOD=6,
	BIND_OP_LSHIFT=7,
	BIND_OP_RSHIFT=8,
	BIND_OP_LAST=7,
} ArgBindingOper;

/* Internal tiler data structures */

typedef enum {
	USE_SPACENAME=0,		/* If this dimension is used by user kernel */
	USE_ITERNAME=1,			/* If this dimension is used by user kernel */
	USE_TOTALITERNAME=2,		/* If this dimension is used by user kernel and as a tiled most inner space of at least one kernel argument, itself double buffered (L2 or L1) */
	USE_LASTITERNAME=3,		/* If this dimension is used by user kernel and as a tiled most inner space of at least one kernel argument */
	USE_NEXTLASTITERNAME=4,		/* If this dimension is used by user kernel and as a tiled most inner space of at least one kernel argument double buffered or not */
	USE_NEXTNEXTLASTITERNAME=5,	/* If this dimension is used by user kernel and as a tiled most inner space of at least one kernel argument double buffered L1 and L2 */
	USE_LAST=6
} InfosUsage_T;

typedef struct AGraphNodeList_T GraphNodeList_T;
typedef struct AGraphNode_T GraphNode_T;
typedef struct AGraphEdgeWeb_T GraphEdgeWeb_T;
typedef struct AGraphEdgeWebList_T GraphEdgeWebList_T;
typedef struct AChannelNodeList_T ChannelNodeList_T;
typedef struct AChannelList_T ChannelList_T;
typedef struct ABufferList_T BufferList_T;

typedef struct {
	char *SpaceName;		/* Name of this space */
	char *IterName;			/* C name for the iterator */
	char *TotalIterName;		/* C name for the total count of iterations so far */
	char *LastIterName;		/* C name to capture if current iteration is the last one */
	char *NextLastIterName;		/* C name to capture if next current iteration is the last one */
	char *NextNextLastIterName;	/* C name to capture if next next current iteration is the last one */
	char Used[USE_LAST];		/* Tracks Kernel's variables usage in order to issue var declarations */
} KernelIteratorInfosT;	// Then we have an KerIterInfos[KER_ITER_LAST] to capture each of them

typedef struct {
	unsigned int InitialDim;	/* Dimension as declared */
	unsigned int TiledDim;		/* Dimension after tiling */
	unsigned int TileSize[2];	/* InitialDim = (TiledDim-1)*TileSize[0] + TileSize[1] */
	unsigned int PreferedDiv;	/* InitialDim = k1*PreferedDiv + k0, k0<PreferedDiv */
	char NoRem;			/* Select space divider with no remainder */
	char Allocated;			/* =1 once this parametric dimension has been set */
} KerIteratorParT;

typedef struct {
	KernelIteratorT KerIterSpace;	/* A tiled Iterator (possibly a single tile) or a regular iterator */
	unsigned int Dim;		/* If KerIterSpace is a regular iterator provides the dimension, otherwise (tiled iterator) it is irrelevant */
	unsigned int Position;		/* In which position in KernelIterationSpace from Outer=0 to IterDim-1 */
	KerIteratorParT *Parameter;	/* In case a non tiled space is parametric */
} KernelIteratorDescrT;


typedef struct {
	unsigned int Dim;		/* Number of items in this particular dimension */
	KernelIteratorT KerIterSpace;	/* A tiled Iterator (possibly a single tile) or a regular iterator, in case of a Ker Arg Sub Space only the most inner dim can be tiled */
	unsigned int SpaceSize[2];	/* Total size of this sub space */
	unsigned int SubSpaceSize[4];	/* In bytes size of one item of this sub space, In case sub space is parametric index 0 is std tile dim, index 1 is last tile dim  */
	char IsAbs;			/* If 1 this subspace is addressable through Base (relative or not) + subscript, if 0 it is relative */
	char InL1;			/* If 1 this subspace is entirely in L1 addressable through Base (relative or not) + subscript */
	char Promoted;			/* If 1 this subspace has been promoted to buffer */
} KernelArgOneDimDescrT;

typedef struct {
	unsigned int Dim;				/* Number of dimensions of this kernel argument */
	unsigned int Size;				/* Total size in bytes of this kernel argument */
	KernelArgOneDimDescrT **DimDescr;		/* A vector of dimension description outer to inner */
	KernelArgOneDimDescrT **IterOrderDimDescr;	/* Reordered DimDescr according to Kernel Iteration Order */
	int *KerIterDimDescr;				/* Indexed by kernel's IterOrder, if in DimDescr then position in IterOrderDimDescr otherwise -1 */
} KernelArgDimDescrT;

typedef struct {
	KernelIterationDimT IterDim;		/* Number of iterators */
	KernelIteratorDescrT **IterOrder;	/* A vector of iterator descriptor from outer to inner */
	KernelIteratorDescrT **Iter;		/* A vector of iterator descriptor indexed by  KernelIteratorT */
} KernelIterationSpaceT;

/* END OF NEW INTERFACE */

typedef struct {
	char *DevVarName;	/* 1 Name of device handler variable, a structure, passed to open as & */
	char *DevVarType;	/* 2 Type Name for DevVarName, assumed to be a structure */

	char *DevVarConfName;	/* 3 Name of the configuration variable to configure the device */
	char *DevVarConfType;	/* 4 Type Name for DevVarConfName, assumed to be a struct so used with & */

	char *DevConfFun;	/* 5 Function name to configure the device before opening it */

	char *DevOpenFun;	/* 6 Function name to open the device */

	char *DevCloseFun;	/* 7 Function name to close the device */

	char *DevRW_1D;		/* 8 Function for a 1D read or write on the device */
	char *DevRW_2D;		/* 9 Function for a 2D read or write on the device */
	char *DevR_Op;		/* 10 Opcode for a read request on the device */
	char *DevW_Op;		/* 11 Opcode for a write request on the device */

	char *DevRWExtType; 	/* 12 Type to be used for the external direction */
	char *DevRWLocType; 	/* 13 Type to be used for the local direction */

	char *DevR_Prefix;	/* 14 Prefix to be used to generate Read Request events */
	char *DevW_Prefix;	/* 15 Prefix to be used to generate Write Request events */
	char *DevRW_ReqType;	/* 16 Type name for the event used to notify completion or a R/W request */
	char *DevRW_Wait;	/* 17 Function for waiting completion of a R/W request */
	char *DevAllocFun;	/* 18 Function to perform dynamic allocation on this device */
	char *DevFreeFun;	/* 19 Function to perform dynamic de allocation on this device */
	char *DevAllocPtrType;	/* 20 Type to be used for malloc / free */
	char IsFlash;		/* 21 1 if this device can be used for remanent strorage */
	char Used;		/* 22 1 if this device is used */
	char ExternalSet;	/* 23 1 if this device is configured and open externally */
	char *DevInitConst;	/* 24 Name to be used to produce an init const image for this device or to be used to open an init const file at runtime */
} Device_T;

typedef struct A_Object_T Object_T;
typedef struct A_Kernel_Arg_T Kernel_Arg_T;

typedef enum {UNDEF_MEM=0, MEM_L3, MEM_L2, MEM_L1, MEM_LAST} MemHierarchy_T;

typedef enum {ARG_SCOPE_UNDEF, ARG_SCOPE_ARG, ARG_SCOPE_ARG_ALLOC, ARG_SCOPE_GLOBAL, ARG_SCOPE_LOCAL} ArgScope_T;
typedef enum {ARG_DIR_UNDEF, ARG_DIR_IN, ARG_DIR_CONSTIN, ARG_DIR_OUT, ARG_DIR_INOUT} ArgDirection_T;
typedef enum {
	AT_MEM_UNDEF,
	AT_MEM_L3_HRAM,
	AT_MEM_L3_QSPIRAM,
	AT_MEM_L3_OSPIRAM,
	AT_MEM_L3_HFLASH,
	AT_MEM_L3_QSPIFLASH,
	AT_MEM_L3_OSPIFLASH,
	AT_MEM_L3_MRAMFLASH,
	AT_MEM_L2,
	AT_MEM_L1,
	AT_MEM_LAST
} AT_MemLocation_T;

#define IS_L3_RAM(Mem) (((Mem)>=AT_MEM_L3_HRAM) && ((Mem)<=AT_MEM_L3_OSPIRAM))
#define IS_FLASH_LOC(Loc) (((Loc)==AT_MEM_L3_HFLASH)||((Loc)==AT_MEM_L3_QSPIFLASH)||((Loc)==AT_MEM_L3_OSPIFLASH)||((Loc)==AT_MEM_L3_MRAMFLASH))
#define IS_VALID_MEM(Mem) (((Mem)>=AT_MEM_L3_HRAM) && ((Mem)<=AT_MEM_L1))

typedef struct {
	char *FileName;	/* Name of the file containing the initial values */
	int Format;	/* Float or fixed point */
	int Binary;	/* 1: file content is binary, 0: file content is text */
	int Size;	/* When Format is Fract Size in bytes of the container */
	int Fract;	/* When format is fract position of the point */
} ConstInit_T;

typedef struct {
	ArgScope_T Scope;		/* ScopeArg: Arg is passed as an argument, ScopeGlobal: Arg is a global constant */
	ArgDirection_T Dir;		/* Arg's direction in, out or in out */
	AT_MemLocation_T HomeLoc;	/* Where the arg permanently exists in the memory hierarchy, can be undefined if the Arg is transient */
	AT_MemLocation_T ExecLoc;	/* Where the arg is located when used by a kernel, can be different than HomeLoc, if undef we decide */
	int UserAllocated;		/* If 1 this symbol should ne considered allocated externally in L2 or L3, if 0 we take care of allocation  */
	int UserExecDefined;		/* 1 if ExecLoc != AT_MEM_UNDEF, exec loc can't be changed by symbol allocator */
	ConstInit_T *Init;		/* How to initialize in case Kernel argument is a constant */
	Kernel_Arg_T *KerArg;		/* In case C arg is referenced into a Ker Arg gives a straight access to this kernel arguement */
	GraphEdgeWeb_T *GraphSymbol;	/* Pointer to related CNN graph argument */
} CArg_Descriptor_T;

#define HAS_ARG_INFO(Arg)	((Arg) && (Arg)->CArg && (Arg)->CArg->ArgInfo)
#define ARG_IN_L2(Arg)		(HAS_ARG_INFO((Arg)) && ((Arg)->CArg->ArgInfo->ExecLoc==AT_MEM_L2))
#define ARG_IN_L3(Arg)		(HAS_ARG_INFO((Arg)) && ((Arg)->CArg->ArgInfo->ExecLoc>=AT_MEM_L3_HRAM) && ((Arg)->CArg->ArgInfo->ExecLoc<=AT_MEM_L3_MRAMFLASH))

typedef struct {
	NameT *Name;			/* The C Template Name */
	NameT *Type;
	KernelArgSelect_T ArgSel;
	unsigned int KernelArg;
	int Value;
	ArgBindingOper Oper;
	NameT *NameKernelArg;		/* When bound to a C arg this is the actual name */
	NameT *ValueKernelArg;		/* When a second C arg is needed */
	KernelIteratorT ItSpace;	/* In case an iterator name is needed */
	CArg_Descriptor_T *ArgInfo;
} CKernel_Arg_T;

typedef enum {GNA_UNDEF, GNA_IN, GNA_OUT, GNA_INOUT} GraghNodeArgT;

typedef struct {
	ArgBindingT BindType;
	GraghNodeArgT GraphNodeType;
	NameT *TargetArgName;
	NameT *AliasTargetArgName;
	NameT *SourceArgName;
	int Value;
	ArgBindingOper Oper;
	NameT *CValueName;
	KernelArgSelect_T ArgSelect;
	CKernel_Arg_T *TargetArgDescr;
	CKernel_Arg_T *AliasTargetArgDescr;
	CKernel_Arg_T *SourceArgDescr;
	KernelIteratorT ItSpace;	/* In case an iterator name is needed */
} ArgBindingDescr_T;

typedef struct {
	NameT *Name;
	KernelCallLocationT CallLocation;
	KernelCallTypeT CallType;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	ArgBindingDescr_T **BindingList;
} CKernelCall_T;

typedef struct {
	NameT *Name;
	int Value;
} KernelDynamicSymbol_T;


typedef struct {
	char *TilePointer[2*CG_MAX_PIPE_DEPTH+1];	/* Name for a tile pointer as a function of pipe depth [-MaxPipe .. MaxPipe] */
	char *TileSize[2*CG_MAX_PIPE_DEPTH+1];		/* Name for a tile size as a function of pipe depth [-MaxPipe .. MaxPipe] */
	char *TileLength[2*CG_MAX_PIPE_DEPTH+1];	/* Name for a tile length for a 2D arg as a function of pipe depth [-MaxPipe .. MaxPipe] */
	char *TileOffset;				/* Name for a tile offset to be added to tile pointer when dereferencing, needed is one of the arg subspace is marked as IsAbs */
	char UsedPointer[2*CG_MAX_PIPE_DEPTH+1];	/* To track usage of tile pointers for proper variable declaration */
	char UsedSize[2*CG_MAX_PIPE_DEPTH+1];		/* To track size of tiles for proper variable declaration */
	char UsedLength[2*CG_MAX_PIPE_DEPTH+1];		/* To tack 2D length of tiles (if arg is 2D) for proper variable declaration */
} KerArgInfos_T;
#define TILE_PTR(PipeOff)	((PipeOff) + CG_MAX_PIPE_DEPTH)

#define NB_TILEABLE_SPACES 3

typedef struct {
	unsigned int L2_Move;
	unsigned int L3_Move;
	float TilingOverhead;
	unsigned int Stride2D;
} KerArgCost_T;

typedef struct A_Kernel_T Kernel_T;
typedef struct A_Kernel_Arg_T {
	NameT        *KerArgName;
	unsigned int Type;
	Tile_Orientation_T Orientation;
	KernelArgDimDescrT *KerArgSpace;
	unsigned int L1Offset;
	unsigned int L2Offset;
	unsigned int L3Offset;
	unsigned int Width;
	unsigned int UsedWidth;
	unsigned int UsedW;
	unsigned int Height;
	unsigned int UsedHeight;
	unsigned int UsedH;
	int Overlap;
	unsigned int DimRatio;
	unsigned int DimOff;
	unsigned int DimRem;
	unsigned int Constraints;
	unsigned int FixDim[2];
	unsigned int VarDim[2];		/* N tiles, N-1 with Dim=VarDim[0], Last one with Dim=VarDim[1] */
	unsigned int Pad[4];
	unsigned int ArgPad[4];
	int ItemSize;
	unsigned int MoveSize[4];	/* [D1][D0] or [D0][T] or [T] D1,D0 parameteric spaces, T tileable space. D1/D0/T=0 Std tile, D1/D0/T=1 Last Tile */
	unsigned int MoveStride;
	unsigned int MoveStride1D[2];
	unsigned int Length2D[2];	/* 0: Standard tile, 1: last tile */
	unsigned int Stride2D;
	unsigned int ArgStride;
	unsigned int BufferSize;
	unsigned int L2BufferSize;
	int BuffCount;
	CKernel_Arg_T *CArg;
	unsigned int Index;
	unsigned char R_Event;
	unsigned char W_Event;
	unsigned char R_Channel;
	unsigned char W_Channel;
	int GraphSymbol;
	KerArgInfos_T *Infos;
	KerArgCost_T *Cost;
	Kernel_T *KerRef;
} Kernel_Arg_T;


typedef struct A_Object_T {
	NameT 	     *KerArgName;
	unsigned int Type;
	Tile_Orientation_T Orientation;
	Object_T *RefObj;
	KernelArgDimDescrT *KerArgSpace;
	unsigned int Width;
	unsigned int UsedWidth;
	unsigned int UsedW;
	unsigned int Height;
	unsigned int UsedHeight;
	unsigned int UsedH;
	unsigned int Pad[4];
	unsigned int ArgPad[4];
	unsigned int ArgStride;
	unsigned int BottomBuffer;
	unsigned int TopBuffer;
	int ItemSize;
	int Overlap;
	unsigned int Alignment;
	unsigned int PreferedTileSize;
	unsigned int PrefRem;		/* Tile size should be Ts = PrefRem + K * PreferedTileSize */
	unsigned int Constraints;
	unsigned int DimOff;
	unsigned int DimRem;
	unsigned int DimRatio;
	float        FDimRatio;
	int BuffCount;
	unsigned int BaseBuffer1;
	unsigned int BaseBuffer2;
	unsigned int BufferSize;
	NameT	     *CArgName;
	union {
		unsigned int ArgN;
		Object_T *Alias;
	} Alias;
	KerArgCost_T *Cost;
} Object_T;

typedef enum {
	AT_KERINFO_OPER,
} AT_KernelInfo_T;

typedef struct {
	unsigned int Oper;
} KernelInfos_T;

typedef struct A_Kernel_T {
	NameT *Name;
	unsigned int First;		/* For Kernel group only */
	unsigned int Last;		/* For Kernel group only */
	unsigned int Instance;
	unsigned int InGroup;
	int IsUsed;
	InlineModeT  InlineMode;
	KernelOptimizationT KerOpt;
	Tile_Orientation_T Orientation;
	// unsigned int Dimension;
	unsigned int Iteration[KER_ITER_TILE_MAX];
	unsigned int RemIteration[KER_ITER_TILE_MAX];
	KernelIterationSpaceT *KernelIterSpace;
	unsigned int ArgCount;
	Kernel_Arg_T **Arg;
	Kernel_Arg_T **RefArg;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	unsigned int CCallsCount;
	CKernelCall_T **CCalls;
	unsigned int BaseIndex;
	unsigned int DynSymbolCount;
	KernelDynamicSymbol_T **DynSymbol;
	unsigned int UsedL1Memory;
	unsigned int UsedL2Memory;
	KernelInfos_T *KerInfos;
} Kernel_T;

typedef struct {
	int TileOrientation;	/* Set Tiling orientation TILE_HOR TILE_VER */
	int ParallelFeatures;	/* Parallelize along channels */
	int ForceDPconv;	/* Forces double precision convolution*/
	int UseHwCE;		/* Enable HW CE */
	AT_PadType PadType;	/* Control padding strategy */
	int EnableIm2Col;	/* Enable mat mul based convolution when feasible */
	int ReluN;		/* if != -1 Overides 6 as a default value for ReLUN */
	int MulBiasScalar;	/* if != -1 Overides default non scalar for MulBias convolutions */
} CNN_GenControl_T;

typedef struct {
	int N_Oper1;		/* Number of primary Kernel operations supported by this user kernel */
	KernelOper_T *KerOper1;	/* List of Matching primary operation, N_Oper1 */
	int N_Oper2;		/* Number of Secondary Kernel operations supported by this user kernel */
	KernelOper_T *KerOper2;	/* List of Matching secondary operation, N_Oper1 */
	int ParallelFeatures;	/* if Non 0 this kernel evaluates features in parallel, if not one feature is evaluated on multiple cores, -1 don't care */
	int OpType[5];		/* 0: In1, 1: In2, 2: In3, 3: Out , For each of them size in bytes or 0 if to be ignored */
	int Fx;			/* Filter x dimension, 0 don't care, -1 any value, >0 a given value */
	int Fy;		 	/* Filter y dimension, 0 don't care, -1 any value, -2 any value but equal to Fy, >0 a given value */
	int Dx;			/* In case of convolution x dilation, 0 don't care, -1 any value, >0 a given value */
	int Dy;			/* In case of convolution y dilation, 0 don't care, -1 any value, -2 any value but equal to Dy, >0 a given value */
	int Sx;			/* Stride in x direction, 0 don't care, -1 any value, >0 a given value */
	int Sy;			/* Stride in x direction, 0 don't care, -1 any value, -2 any value but equal to Sy, >0 a given value */
} CNN_LayerOp_T;

typedef struct {
	NameT *Name;
	KernelCallTypeT CallType;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	NameT *ParArgTypeName;
	CNN_LayerOp_T *Oper;
} KernelLib_T;

#if 0
typedef struct {
	NameT *Name;
	unsigned int Instance;
	unsigned int First;
	unsigned int Last;
	unsigned int InGroup;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	unsigned int CCallsCount;	/* Should be equal to Last-First+1 */
	CKernelCall_T **CCalls;
} KernelGroup_T;
#endif

typedef Kernel_T KernelGroup_T;

typedef struct {
	NameT *Name;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
} KernelLibTemplate_T;

typedef struct A_MemChunk_T MemChunk_T;
typedef struct A_MemChunk_T {
        unsigned int Base;
        unsigned int Size;
        MemChunk_T *Next;
} MemChunk_T;

typedef enum {NODE_ENTRY, NODE_USER_KER, NODE_USER_KER_GROUP, NODE_EXIT} NodeType_T;

typedef struct AChannelNodeList_T {
	GraphNodeList_T *List;			/* List of graph nodes in this channel */
	int ChannelDepth;			/* Channel depth from interference graph point of view */
	int ChannelIndex;			/* This channel */
	unsigned int *Symbols; 			/* Which symbol are alive in this channel */
	unsigned int FirstNode;			/* First node index of this channel */
	unsigned int LastNode;			/* Last node index of this channel */
	MemHierarchy_T Memory[MEM_LAST]; 	/* What this channel needs for each memory level */
	unsigned int *MemoryNode[MEM_LAST];	/* Memory node per node */
	int Base[MEM_LAST];			/* Base Address of this channel for each memory level */
} ChannelNodeList_T;

typedef struct AChannelList_T {
	ChannelNodeList_T *Channel;
	ChannelList_T *Next;
} ChannelList_T;

typedef struct {
	int NChannels;
	ChannelList_T *Channels;
} StackedChannels_T;

typedef struct AGraphNodeList_T {
	GraphNode_T *Node;			/* Source or desctination node */
	Kernel_Arg_T *KerArg;			/* Corresponding Kernel Argument in related binding */
	ArgBindingDescr_T *Binding;		/* The bindings from which this edge is originating */
	GraphEdgeWeb_T *Web;			/* Which symbol */
	unsigned int Size;			/* Size of this symbol as seen in the related kernel argument */
	int Offset;				/* Offset applied to the  base of this symbol in case binding Oper is + or - */
	int Channel;				/* To which channel this symbol belongs to */
	int ChannelDepth;			/* Channel depth */
	GraphNodeList_T *Next;			/* Next edge */
} GraphNodeList_T;

typedef struct AGraphEdgeWebList_T {
	GraphEdgeWeb_T *Edge;
	GraphEdgeWebList_T *Next;
} GraphEdgeWebList_T;

typedef struct {
	AT_MemLocation_T MemType;
	unsigned int Address;
	unsigned int Size;
} MemLoc_T;

typedef struct ABufferList_T {
	unsigned int NodeIndex;
	Kernel_Arg_T *KerArg;
	MemLoc_T *Exec;
	BufferList_T *Next;
} BufferList_T;

typedef struct {
	GraphEdgeWeb_T *Symbol;
	MemLoc_T *Install;		/* For constant, where are they stored at install time, should be h flash, q flash or emram*/
	MemLoc_T *Load;			/* For constant, where are they loaded when the graph is instanciated */
	MemLoc_T *Exec;			/* How is this symbol seen from user kernel point of view (l2 or l3) */
	BufferList_T *Buffers;		/* List of L2 buffers related to this symbol */
	int LiveFirst;			/* First node in Graph where symbol is alive */
	int LiveLast;			/* Last node in Graph where symbol is alive */
	int Event;			/* Event number to be used if a move of this symbol is needed */
} SymbolAlloc_T;

typedef struct AGraphEdgeWeb_T {
	CKernel_Arg_T *Edge;		/* The symbol, CArgs or Locals in the current graph */
	unsigned int Index;		/* Index of this Symbol */
	unsigned int Size;		/* Size of this symbol */
	int LiveFirst;			/* Graph node index of start life for this symbol */
	int LiveLast;			/* Graph node index of start stop for this symbol */
	Kernel_Arg_T *KerArg;		/* This symbol is bounded to this Kernel argument */
	GraphNodeList_T *From;		/* Source graph nodes producing this symbol */
	GraphNodeList_T *To;		/* Destination graph nodes reading this symbol */
	GraphEdgeWebList_T *Equiv;	/* List of equivalent symbols coming from GNA_INOUT propertiy */
	GraphEdgeWeb_T *Next;		/* Next Symbol */
	SymbolAlloc_T *Alloc;		/* Memory allocation infos */
} GraphEdgeWeb_T;

typedef struct AGraphNodeCalls_T GraphNodeCalls_T;
typedef struct AGraphNodeCalls_T {
	NameT *FunName;
	ArgBindingDescr_T **FunArgs;
	int FunArgsCount;
	GraphNodeCalls_T *Next;
} GraphNodeCalls_T;

typedef struct AGraphNode_T {
	NodeType_T Type;		/* Node type: user kernel or user kernel group */
	NameT *Name;			/* User kernel or user kernel group name */
	int Index;			/* Unique index for this node */
	int Channel;			/* To which allocation group this node belongs to */
	int ChannelDepth;		/* Number-1 of stacked channels on this node */
	Kernel_T *Obj;			/* User Kernel or User Kernel group */
        ArgBindingDescr_T **BindingList;/* Graph in out binding list */
	unsigned int BindCount;		/* Number of binding */
	GraphNodeCalls_T *PrologCalls;	/* List of functions to be called before this node is evaluated */
	GraphNodeCalls_T *EpilogCalls;	/* List of functions to be called after this node is evaluated */
	Kernel_Arg_T **KerArg;		/* Sub set of BindingList pointing to non C or immediate a kernel argument */
	int *KerArgToNodeArg;		/* For KerArg access to original BindingList */
	unsigned int ArgCount;		/* Number of argument */
	unsigned int *AliveAllSymb;	/* Bit vector to capture which symbol, local or graph arg, is alive at this point */
	unsigned int *AliveSymb;	/* Bit vector to capture which symbol, local or graph arg excluding const in, is alive at this point */
	MemHierarchy_T Memory[MEM_LAST];	/* Minimum memory (channel memory) for this graph node, sum of Node variables plus alive variables at this point */
	MemHierarchy_T OwnMemory[MEM_LAST];	/* Minimum memory (channel memory) for this graph node, sum of Node variables */
	GraphNodeList_T *Succ;		/* Successors of this node */
	GraphNode_T *Next;		/* Next node */
} GraphNode_T;

typedef struct {
	NameT *Name;
	CKernel_Arg_T **CArgs;			/* The C arguments of the CNN graph as a callable function */
	unsigned int CArgsCount;		/* Number of C args */
	CKernel_Arg_T **Locals;			/* CNN graph C local arguments, will have to be dynamically allocated */
	unsigned int LocalsCount;		/* Number of locals */
	GraphNode_T *Nodes;			/* User Kernels or user kernel groups list */
	GraphNode_T **OrderedNodes;		/* User Kernels or user kernel groups array after toplogical sorting */
	unsigned int NodesCount;		/* Number of nodes */
	GraphEdgeWeb_T **EdgesWeb;		/* Graph variables [CArgsCount+LocalsCount] organized as a web (from node list, to node list) */
	ChannelNodeList_T **Channels;		/* Groups of nodes for allocation, indexed by Channel index */
	unsigned int NChannels;			/* Number of channels */
	StackedChannels_T *StackedChannels; 	/* Interference graph between channels */
	unsigned int MaxChannelDepth;		/* Maximum number of stacked channels */
} CNNGraph_T;

typedef enum {
	AT_KERNEL_BUFFER_PROMOTE=1,		/* When all user kernel arguments can fit into given L1 memory promote them to buffer, default is 1 */
	AT_KERNEL_PARTIAL_BUFFER_PROMOTE,	/* When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer, default is 1 */
	AT_KERNEL_NOSOLUTION_ERROR,		/* Report an error when no tiling solution is found, default is 1 */
	AT_GRAPH_MONITOR_CYCLES,		/* Enable automatic cycle capture for each node of the graph, default is 0 */
	AT_GRAPH_MONITOR_CVAR_NAME,		/* When monitor cycles is on name of the C var array to receive results, default is AT_GraphPerf */
	AT_GRAPH_PRODUCE_NODE_NAMES,		/* Enable production of an array containing the name of each graph node, default is 0 */
	AT_GRAPH_PRODUCE_NODE_CVAR_NAME,	/* When producing node names is on name of the C array receiving the names as strings, default is AT_GraphNodeNames */
	AT_GRAPH_PRODUCE_OPERINFOS,		/* Enable production of number of macs for each layer, default is 0 */
	AT_GRAPH_PRODUCE_OPERINFOS_CVAR_NAME,	/* When Number of oper Infos is on name of the C array receiving mac infos for each node, default is AT_GraphOperInfosNames */
	AT_GRAPH_REORDER_CONSTANT_IN,		/* Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses, default is 1 */
	AT_GRAPH_TRACE_EXEC,			/* Enable trace of activity, default is 1 */
	AT_GRAPH_NOINLINE_NODE,			/* If 1 all user kernel function is marked as noinline, default is 0 */
	AT_GRAPH_PREF_L3_EXEC,			/* In case a symbol must be allocated in L3 for execution this is the prefered memory, default is AT_MEM_L3_HRAM */
	AT_GRAPH_CONST_EXEC_FROM_FLASH,		/* If 1, for constant symbol executes from home location, default is 0 */
	AT_GRAPH_PREF_L3_HOME,			/* For constant symbols which L3 flash prefered memory, default is AT_MEM_L3_HFLASH */
} AT_GraphCtrl_T;
/*
#define AT_OPT_ON	((void *) 1)
#define AT_OPT_OFF	((void *) 0)
#define AT_OPT_VAL(Val)	((void *) (Val))
*/
typedef struct {
	int KerBufferPromote;			/* When all user kernel arguments can fit into given L1 memory promote them to buffer */
	int KerPartialBufferPromote;		/* When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer */
	int KerNoTilingSolutionError;		/* Report an error when no tiling solution is found */
	int MonitorCycles;			/* 1 if cycles should be captured for each node of the graph */
	char *C_TimerName;			/* Name of the C array variable 0..LastNode to keep the time, will be declared and defined */
	int ProduceNodeNames;			/* 1 if node names should be assigned to an array of string for dump purpose at graph execution ti,e */
	char *C_NodeName;			/* Name of the C array variable where to assign node name, will be declared and defined */
	int ProduceOperInfos;			/* Generate mac count information for each layer */
	char *C_OperInfosName;			/* Name of the C array varaible where to assign oper count info for each node, will be declared and defined */
	int ReorderConstTensor;			/* For constant tensors rewrite them folloing tile order in order to transform 2D accesses into linear accesses */
	int TraceExec;				/* 1 if log for graph exec is active */
	int NoNodeInline;			/* if 1 all user kernel function is marked as noinline */
	AT_MemLocation_T PreferedL3ExecLoc;	/* In case a symbol must be allocated in L3 in which memory, default is AT_MEM_L3_HRAM */
	int ConstExecFromFlash;			/* If 1, for constant symbol executes from home location, default is 0 */
	AT_MemLocation_T PreferedConstL3HomeLoc;/* For a constant symbol where to store it L3, default is AT_MEM_L3_HFLASH */
} GraphControl_T;

#define Q2F(V, N)               ((float) (((float) (V))/((1<<(N))-0)))
#define MultRndu(x,y, scale)    ((unsigned int)(((x)*(y)) + (1<<((scale)-1)))>>(scale))
#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

/* Return aligned value, alignment is 2^Size */
#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

extern KernelIteratorInfosT KerIterInfos[];

extern	   KernelLibTemplate_T KernelLibTemplate[];
extern	   KernelLib_T KernelLib[];
extern int HeadKernelLib;
extern int HeadKernelLibTemplate;
extern     Kernel_T KernelStack[];
extern int HeadKernelStack;

extern     KernelGroup_T KernelGroup[];
extern int HeadKernelGroup;

extern CNNGraph_T *CurCNNGraph;

extern unsigned int L1MemorySize, L2MemorySize, L3MemorySize;
extern int MaxL1TopMemory;
extern int MaxAllocatedL2;
extern char *OutputDir;
extern char *ConstDir;
extern char *FlashDir;
extern char *LibTemplateName;

extern Device_T L3_L2_Device[];
extern Device_T L3_L2_Device_FC[];
extern Device_T L2_L1_Device;
extern Device_T L1_Device;
extern char *MemoryBaseName[];
extern int TopAllocatedMemory[];
extern char *L1MemoryBase;

extern GraphControl_T GraphControl;

extern int TopAllocatedMemory[];
extern int AvailableMemory[];

extern char *ConstDirName;


/// @endcond

/** @}*/ // End of group AutoTilerTypes

/* Data type for non inlined code generation, used at runtime */

typedef struct {
        int DimRatio;
        unsigned int L2Offset;
        unsigned short int L1Offset;
        unsigned short int FixDim[2];
        unsigned short int VarDim[2];
        unsigned short int MoveSize[2];
        unsigned short int MoveStride;
        unsigned short int Length2D[2];
        unsigned short int Stride2D;
        unsigned short int BufferSize;
        unsigned short int BuffCount;
} Kernel_Arg_Exec_T;

typedef struct {
        unsigned char Iteration[4];
        unsigned char ArgCount;
        unsigned char NinPlanes;
        unsigned char NoutPlanes;
        Kernel_Arg_Exec_T *Arg;
} Kernel_Exec_T;

/* For bit vector manipulation */
#define BIT_S(Vect, Pos)        ((Vect)[(Pos)>>5] = ((Vect)[(Pos)>>5] |  (1<<((Pos)&31))))
#define BIT_C(Vect, Pos)        ((Vect)[(Pos)>>5] = ((Vect)[(Pos)>>5] & ~(1<<((Pos)&31))))
#define BIT_V(Vect, Pos)        (((Vect)[(Pos)>>5] &  (1<<((Pos)&31)))!=0)


/* For rounding up */

#define RND_UP(Val, Div)	(((Val) + ((Div)-1))/(Div))

#endif
