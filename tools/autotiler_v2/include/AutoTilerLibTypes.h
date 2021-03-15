/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __GEN_TILING_H__
#define __GEN_TILING_H__

#include "HashName.h"

/** @defgroup AutoTilerTypes AutoTilerDataTypes
@ingroup groupAutoTiler
@{ */

/* Visible to tiler API */

typedef   signed char  v4s __attribute__((vector_size (4)));

typedef enum {
	K_INP = 0,
	K_OUTP = 1,
	K_W = 2,
	K_H = 3,
	K_DIM_LAST = 4,
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
	KER_ITER_NONE=0,	/**< No iteration space */
	KER_TILE=1,		/**< Most inner iteration space operating on the basic data plane of a user kernel argument */
	KER_TILE1=2,		/**< Second inner iteration space operating on the basic data plane of a user kernel argument */
	KER_TILE2=3,		/**< Third inner iteration space operating on the basic data plane of a user kernel argument */
	KER_IN_PLANE=4,		/**< In Planes iteration space */
	KER_OUT_PLANE=5,	/**< Out Planes iteration space */
	KER_IN_OUT_PLANE=6,	/**< Out and In Planes are traversed in the same iteration space, NinPlanes has to be equal to NoutPlanes */
	KER_ITER_LAST=7		/**< Marker for last */
} KernelIteratorT;
/// @cond PrivateStuff
#define KER_TILE_MAX 3
#define IterSpaceToTile(Space) (((Space)==0)?O_TILE0:(((Space)==1)?O_TILE1:(((Space)==2)?O_TILE2:0)))

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
} KernelCallTypeT;

/**
@brief Call location attribute
*/
typedef enum {
	CALL_IN=0,		/**< Call in the iterator body */
	CALL_PROLOG=1,		/**< Call before the iterator starts */
	CALL_EPILOG=2,		/**< Call after the iterator stops */
} CallLocationT;

/**
@brief Call location for basic kernels in the user kernel defined iteration space

Call location for basic kernels in the user kernel defined iteration space

When the context is a group of user kernels called kernels are user kernels and not basic kernels and in this case only LOC_IN_GROUP is valid
*/
typedef enum {
	LOC_INNER_LOOP=0,		/**< Call basic kernel in the loop body of iterator 0 on the basic data planes */
	LOC_INNER_LOOP_PROLOG=1,	/**< Call basic kernel right before the first iteration of iterator 0 on the basic data planes */
	LOC_INNER_LOOP_EPILOG=2,	/**< Call basic kernel right after the last iteration of iterator 0 on the basic data planes */

	LOC_INNER_LOOP1=3,		/**< Call basic kernel in the loop body of iterator 1 on the basic data planes  */
	LOC_INNER_LOOP1_PROLOG=4,	/**< Call basic kernel right before the first iteration of iterator 2 on the basic data planes */
	LOC_INNER_LOOP1_EPILOG=5,	/**< Call basic kernel right after the last iteration of iterator 2 on the basic data planes */

	LOC_INNER_LOOP2=6,		/**< Call basic kernel in the loop body of iterator 2 on the basic data planes */
	LOC_INNER_LOOP2_PROLOG=7,	/**< Call basic kernel right before the first iteration of iterator 2 on the basic data planes */
	LOC_INNER_LOOP2_EPILOG=8,	/**< Call basic kernel right after the last iteration of iterator 2 on the basic data planes */

	LOC_IN_PLANE=9,			/**< Call basic kernel inside in planes iteration body */
	LOC_IN_PLANE_PROLOG=10,		/**< Call basic kernel before 1st iteration on in planes */
	LOC_IN_PLANE_EPILOG=11,		/**< Call basic kernel after last iteration on in planes */

	LOC_OUT_PLANE=12,		/**< Call basic kernel inside out planes iteration body */
	LOC_OUT_PLANE_PROLOG=13,	/**< Call basic kernel before 1st iteration on out planes */
	LOC_OUT_PLANE_EPILOG=14,	/**< Call basic kernel after last iteration on out planes */

	LOC_IN_OUT_PLANE=15,		/**< Call basic kernel inside in out planes iteration body */
	LOC_IN_OUT_PLANE_PROLOG=16,	/**< Call basic kernel before 1st iteration on in out planes */
	LOC_IN_OUT_PLANE_EPILOG=17,	/**< Call basic kernel after last iteration on in out planes */

	LOC_PROLOG=18,			/**< Call basic kernel before the entire user kernel iteration structure */
	LOC_EPILOG=19,			/**< Call basic kernel after the entire user kernel iteration structure */

	LOC_GROUP=20,			/**< Call user kernel in a kernel group */
} KernelCallLocationT;

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
	KER_ARG_TILE_H = 6,		/**< Current tile hieght */
	KER_ARG_TILE_W0 = 7,		/**< Tile standard width (not the one for the last iteration that is potentially smaller) */
	KER_ARG_TILE_H0 = 8,		/**< Tile standard height (not the one for the last iteration that is potentially smaller) */
	KER_ARG = 9,			/**< User kernel argument name */
	KER_ARG_W = 10,			/**< User kernel argument width */
	KER_ARG_H = 11,			/**< User kernel argument height */
	KER_ARG_NTILES = 12,		/**< Number of tiles for related user kernel argument */
	KER_ARG_TILEINDEX = 13,		/**< Current tile index for related user kernel argument, starts at 0 */
	KER_ARG_TILE_BASE = 14,		/**< Current tile base in line or column unit, when argument is dynamic it is computed at runtime */
	KER_ARG_INPLANEINDEX = 15,	/**< Current Input Plane index for related user kernel argument, starts at 0 */
	KER_ARG_OUTPLANEINDEX = 16,	/**< Current Output Plane index for related user kernel argument, starts at 0 */
	KER_ARG_PAD = 17,		/**< Actual padding of plane associated to arg (left,right,top,bottom) as a v4s */
	KER_ARG_TILE_PAD = 18,		/**< Actual padding of tile associated to arg (left,right,top,bottom) as a v4s */

	TC_ARG = 19,			/**< A C argument */
	TC_IMM = 20,			/**< An immediate int value */
	TC_USYMB = 21,			/**< A user defined symbol */
	TC_KDIM = 22,			/**< One of the user Kernel Dimensions: InPlane/OutPlane/W/H */
	TC_ARG_IND = 23,		/**< An indirection on a C argument */
	TC_ARG_IND_IP = 24,		/**< A C argument subscripted by the current In Data Plane iteration index */
	TC_ARG_IND_OP = 25,		/**< A C argument subscripted by the current Out Data Plane iteration index */
	TC_ARG_IND_TILE = 26,		/**< A C argument subscripted by the current current most inner iteration index on argument basic data plane */
	TC_ARG_PLUS_IND_IP = 27,	/**< A C argument added to the current In Data Plane iteration index multiplied by a constant */
	TC_ARG_PLUS_IND_OP = 28,	/**< A C argument added to the current Out Data Plane iteration index multiplied by a constant*/
	TC_ARG_PLUS_IND_TILE = 29,	/**< A C argument added to the current current most inner iteration index on argument basic data plane multiplied by a constant */
	TC_ARG_PLUS_OFFSET = 30,	/**< A C argument plus an immediate offset */
	TC_ARG_PLUS_C_OFFSET = 31,	/**< A C argument plus a C variable name offset */
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
	O_IN			= (1<<0),	/**< Argument is an input */
	O_NIN			= (1<<1),	/**< Argument is not an input */
	O_OUT			= (1<<2),	/**< Argument is an output */
	O_NOUT			= (1<<3),	/**< Argument is not an output */
	O_BUFF			= (1<<4),	/**< Argument is a buffer */
	O_NBUFF			= (1<<5),	/**< Argument is not a buffer */
	O_TILED			= (1<<6),	/**< Argument should be tiled */
	O_NTILED		= (1<<7),	/**< Argument should not be tiled */
	O_ONETILE		= (1<<8),	/**< Argument should be exactly one tile in shared L1 */
	O_NONETILE		= (1<<9),	/**< Argument should can have multiple tiles in shared L1 */
	O_DB			= (1<<10),	/**< Argument should be double buffered (or triple) */
	O_NDB			= (1<<11),	/**< Argument should be single buffered */
	O_L2DB			= (1<<12),	/**< Argument is in L3 and should be double or triple buffered in L2 */
	O_NL2DB			= (1<<13),	/**< Argument is not in L3 */
	O_3D			= (1<<14),	/**< Argument traverses the basic data plane (2D) and either input or output planes */
	O_N3D			= (1<<15),	/**< Argument traverse only the basic data plane (2D) */
	O_4D			= (1<<16),	/**< Argument traverses the basic data plane (2D), the input and the output data planes */
	O_N4D			= (1<<17),	/**< Argument does not traverse all iteration spaces */
	O_PBUFF			= (1<<18),	/**< Argument is promoted to partial buffer, a full line of tiles accross input planes */
	O_NPBUFF		= (1<<19),	/**< Argument is not promoted to partial buffer */
	O_ALIAS			= (1<<20),	/**< Argument is aliased to another argument, different name but same memory location */
	O_NALIAS		= (1<<21),	/**< Argument is not aliased */
	O_DYNTILE		= (1<<22),	/**< Argument tile size is adjusted dynamically */
	O_NDYNTILE		= (1<<23),	/**< Argument tile size is not adjusted dynamically */

	O_TILE2			= (1<<29),	/**< Argument traverses the 3rd level of iteration on the basic data plane */
	O_TILE1			= (1<<30),	/**< Argument traverses the 2nd level of iteration on the basic data plane */
	O_TILE0			= (1<<31),	/**< Argument traverses the 1st level of iteration on the basic data plane, this should always be the case if tiled */
} BasicObjectType_T;
/*
#define	O_IN			(1<<0)
#define	O_NIN			(1<<1)
#define	O_OUT			(1<<2)
#define	O_NOUT			(1<<3)
#define	O_BUFF			(1<<4)
#define	O_NBUFF			(1<<5)
#define	O_TILED			(1<<6)
#define	O_NTILED		(1<<7)
#define	O_ONETILE		(1<<8)
#define	O_NONETILE		(1<<9)
#define	O_DB			(1<<10)
#define	O_NDB			(1<<11)
#define	O_L2DB			(1<<12)
#define	O_NL2DB			(1<<13)
#define	O_3D			(1<<14)
#define	O_N3D			(1<<15)
#define	O_4D			(1<<16)
#define	O_N4D			(1<<17)
#define O_PBUFF			(1<<18)
#define O_NPBUFF		(1<<19)
#define	O_ALIAS			(1<<20)
#define	O_NALIAS		(1<<21)
#define	O_DYNTILE		(1<<22)
#define	O_NDYNTILE		(1<<23)

#define O_TILE2			(1<<29)
#define O_TILE1			(1<<30)
#define O_TILE0			(1<<31)
*/
/// @cond PrivateStuff
#define	O_TILEMASK		(O_TILE0|O_TILE1|O_TILE2)
#define OBJ_ITER(Obj)		((Obj)->Type & (O_TILEMASK))
/// @endcond
/*
	OBJ[_IN,_OUT, _IN_OUT]{_DB}{_L2DB}{_NTILED}{_3D}{_4D}
	OBJ_BUFFER{_IN}{_OUT}{_L2DB}{_NTILED}{_3D}{_4D}
	OBJ_BUFFER_ONETILE
*/

/**
@brief Pre defined user kernel argument types

Pre defined user kernel argument types
*/
typedef enum {
	OBJ_IN 				= O_IN|O_TILED,
	OBJ_IN_3D 			= O_IN|O_3D|O_TILED,
	OBJ_IN_4D 			= O_IN|O_3D|O_TILED|O_4D,
	OBJ_IN_NTILED_4D 		= O_IN|O_3D|O_4D,
	OBJ_IN_DB 			= O_IN|O_DB|O_TILED,
	OBJ_IN_DB_3D 			= O_IN|O_DB|O_3D|O_TILED,
	OBJ_IN_DB_4D 			= O_IN|O_DB|O_3D|O_TILED|O_4D,
	OBJ_IN_DB_NTILED_4D 		= O_IN|O_DB|O_3D|O_4D,
	OBJ_IN_L2DB 			= O_IN|O_L2DB|O_TILED,
	OBJ_IN_L2DB_3D 			= O_IN|O_L2DB|O_3D|O_TILED,
	OBJ_IN_L2DB_4D 			= O_IN|O_L2DB|O_3D|O_TILED|O_4D,
	OBJ_IN_L2DB_NTILED_4D 		= O_IN|O_L2DB|O_3D|O_4D,
	OBJ_IN_DB_L2DB 			= O_IN|O_DB|O_L2DB|O_TILED,
	OBJ_IN_DB_L2DB_3D 		= O_IN|O_DB|O_L2DB|O_3D|O_TILED,
	OBJ_IN_DB_L2DB_4D 		= O_IN|O_DB|O_L2DB|O_3D|O_TILED|O_4D,
	OBJ_IN_DB_L2DB_NTILED_4D 	= O_IN|O_DB|O_L2DB|O_3D|O_4D,

	OBJ_OUT 			= O_OUT|O_TILED,
	OBJ_OUT_3D 			= O_OUT|O_3D|O_TILED,
	OBJ_OUT_4D 			= O_OUT|O_TILED|O_4D,
	OBJ_OUT_DB 			= O_OUT|O_DB|O_TILED,
	OBJ_OUT_DB_3D 			= O_OUT|O_DB|O_3D|O_TILED,
	OBJ_OUT_DB_4D 			= O_OUT|O_DB|O_TILED|O_4D,
	OBJ_OUT_L2DB 			= O_OUT|O_L2DB|O_TILED,
	OBJ_OUT_L2DB_3D 		= O_OUT|O_L2DB|O_3D|O_TILED,
	OBJ_OUT_L2DB_4D 		= O_OUT|O_L2DB|O_TILED|O_4D,
	OBJ_OUT_DB_L2DB 		= O_OUT|O_DB|O_L2DB|O_TILED,
	OBJ_OUT_DB_L2DB_3D 		= O_OUT|O_DB|O_L2DB|O_3D|O_TILED,
	OBJ_OUT_DB_L2DB_4D 		= O_OUT|O_DB|O_L2DB|O_TILED|O_4D,

	OBJ_OUT_DB_ALIAS 		= O_OUT|O_DB|O_ALIAS|O_TILED,

	OBJ_IN_OUT 			= O_IN|O_OUT|O_TILED,
	OBJ_IN_OUT_3D 			= O_IN|O_OUT|O_3D|O_TILED,
	OBJ_IN_OUT_4D 			= O_IN|O_OUT|O_3D|O_TILED|O_4D,
	OBJ_IN_OUT_NTILED_4D 		= O_IN|O_OUT|O_3D|O_4D,
	OBJ_IN_OUT_DB 			= O_IN|O_OUT|O_DB|O_TILED,
	OBJ_IN_OUT_DB_3D 		= O_IN|O_OUT|O_DB|O_3D|O_TILED,
	OBJ_IN_OUT_DB_4D 		= O_IN|O_OUT|O_DB|O_3D|O_TILED|O_4D,
	OBJ_IN_OUT_DB_NTILED_4D 	= O_IN|O_OUT|O_DB|O_3D|O_4D,
	OBJ_IN_OUT_L2DB 		= O_IN|O_OUT|O_L2DB|O_TILED,
	OBJ_IN_OUT_L2DB_3D 		= O_IN|O_OUT|O_L2DB|O_3D|O_TILED,
	OBJ_IN_OUT_L2DB_4D 		= O_IN|O_OUT|O_L2DB|O_3D|O_TILED|O_4D,
	OBJ_IN_OUT_L2DB_NTILED_4D 	= O_IN|O_OUT|O_L2DB|O_3D|O_4D,
	OBJ_IN_OUT_DB_L2DB 		= O_IN|O_OUT|O_DB|O_L2DB|O_TILED,
	OBJ_IN_OUT_DB_L2DB_3D 		= O_IN|O_OUT|O_DB|O_L2DB|O_3D|O_TILED,
	OBJ_IN_OUT_DB_L2DB_4D 		= O_IN|O_OUT|O_DB|O_L2DB|O_3D|O_TILED|O_4D,
	OBJ_IN_OUT_DB_L2DB_NTILED_4D 	= O_IN|O_OUT|O_DB|O_L2DB|O_4D,


	OBJ_BUFFER			= O_BUFF|O_TILED,
	OBJ_BUFFER_3D			= O_BUFF|O_3D|O_TILED,
	OBJ_BUFFER_4D			= O_BUFF|O_3D|O_TILED|O_4D,
	OBJ_BUFFER_NTILED		= O_BUFF,
	OBJ_BUFFER_NTILED_3D		= O_BUFF|O_3D,
	OBJ_BUFFER_NTILED_4D		= O_BUFF|O_3D|O_4D,

	OBJ_BUFFER_IN			= O_BUFF|O_IN|O_TILED,
	OBJ_BUFFER_IN_3D		= O_BUFF|O_IN|O_3D|O_TILED,
	OBJ_BUFFER_IN_4D		= O_BUFF|O_IN|O_3D|O_TILED|O_4D,
	OBJ_BUFFER_IN_NTILED		= O_BUFF|O_IN,
	OBJ_BUFFER_IN_NTILED_3D		= O_BUFF|O_IN|O_3D,
	OBJ_BUFFER_IN_NTILED_4D		= O_BUFF|O_IN|O_3D|O_4D,

	OBJ_BUFFER_OUT			= O_BUFF|O_OUT|O_TILED,
	OBJ_BUFFER_OUT_3D		= O_BUFF|O_OUT|O_3D|O_TILED,
	OBJ_BUFFER_OUT_4D		= O_BUFF|O_OUT|O_TILED|O_4D,
	OBJ_BUFFER_OUT_NTILED		= O_BUFF|O_OUT,
	OBJ_BUFFER_OUT_NTILED_3D	= O_BUFF|O_OUT|O_3D,
	OBJ_BUFFER_OUT_NTILED_4D	= O_BUFF|O_OUT|O_4D,

	OBJ_BUFFER_IN_OUT		= O_BUFF|O_IN|O_OUT|O_TILED,
	OBJ_BUFFER_IN_OUT_3D		= O_BUFF|O_IN|O_OUT|O_3D|O_TILED,
	OBJ_BUFFER_IN_OUT_4D		= O_BUFF|O_IN|O_OUT|O_3D|O_TILED|O_4D,
	OBJ_BUFFER_IN_OUT_NTILED	= O_BUFF|O_IN|O_OUT,
	OBJ_BUFFER_IN_OUT_NTILED_3D	= O_BUFF|O_IN|O_OUT|O_3D,
	OBJ_BUFFER_IN_OUT_NTILED_4D	= O_BUFF|O_IN|O_OUT|O_3D|O_4D,

	OBJ_BUFFER_IN_L2DB		= O_BUFF|O_IN|O_L2DB|O_TILED,
	OBJ_BUFFER_IN_L2DB_3D		= O_BUFF|O_IN|O_3D|O_L2DB|O_TILED,
	OBJ_BUFFER_IN_L2DB_4D		= O_BUFF|O_IN|O_3D|O_L2DB|O_TILED|O_4D,

	OBJ_BUFFER_IN_L2DB_NTILED	= O_BUFF|O_IN|O_L2DB,
	OBJ_BUFFER_IN_L2DB_NTILED_3D	= O_BUFF|O_IN|O_3D|O_L2DB,
	OBJ_BUFFER_IN_L2DB_NTILED_4D	= O_BUFF|O_IN|O_3D|O_L2DB|O_4D,

	OBJ_BUFFER_OUT_L2DB		= O_BUFF|O_OUT|O_L2DB|O_TILED,
	OBJ_BUFFER_OUT_L2DB_3D		= O_BUFF|O_OUT|O_3D|O_L2DB|O_TILED,
	OBJ_BUFFER_OUT_L2DB_4D		= O_BUFF|O_OUT|O_L2DB|O_TILED|O_4D,

	OBJ_BUFFER_OUT_L2DB_NTILED	= O_BUFF|O_OUT|O_L2DB,
	OBJ_BUFFER_OUT_L2DB_NTILED_3D	= O_BUFF|O_OUT|O_3D|O_L2DB,
	OBJ_BUFFER_OUT_L2DB_NTILED_4D	= O_BUFF|O_OUT|O_L2DB|O_4D,

	OBJ_BUFFER_IN_OUT_L2DB		= O_BUFF|O_IN|O_OUT|O_L2DB|O_TILED,
	OBJ_BUFFER_IN_OUT_L2DB_3D	= O_BUFF|O_IN|O_OUT|O_3D|O_L2DB|O_TILED,
	OBJ_BUFFER_IN_OUT_L2DB_4D	= O_BUFF|O_IN|O_OUT|O_3D|O_L2DB|O_TILED|O_4D,

	OBJ_BUFFER_IN_OUT_L2DB_NTILED	= O_BUFF|O_IN|O_OUT|O_L2DB,
	OBJ_BUFFER_IN_OUT_L2DB_NTILED_3D= O_BUFF|O_IN|O_OUT|O_3D|O_L2DB,
	OBJ_BUFFER_IN_OUT_L2DB_NTILED_4D= O_BUFF|O_IN|O_OUT|O_3D|O_L2DB|O_4D,

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
	BIND_OP_LAST=7,
} ArgBindingOper;

/* Internal tiler data structures */
/// @cond PrivateStuff
typedef struct {
	unsigned int InPlanes;
	unsigned int OutPlanes;
	unsigned int W;
	unsigned int H;
} KernelDimensionT;


typedef struct {
	KernelIterationDimT IterDim;
	KernelIteratorT *IterOrder;
} KernelIterationT;

typedef struct {
	NameT *Name;			/* The C Template Name */
	NameT *Type;
	KernelArgSelect_T ArgSel;
	unsigned int KernelArg;
	int Value;
	ArgBindingOper Oper;
	NameT *ValueKernelArg;
	NameT *NameKernelArg;		/* When bound to a C arg this is the actual name */
} CKernel_Arg_T;

typedef struct {
	NameT *TargetArgName;
	ArgBindingT BindType;
	NameT *SourceArgName;
	int Value;
	ArgBindingOper Oper;
	NameT *CValueName;
	KernelArgSelect_T ArgSelect;
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

typedef struct A_Object_T Object_T;
typedef struct A_Kernel_Arg_T Kernel_Arg_T;

typedef struct A_Kernel_Arg_T {
	NameT        *KerArgName;
	unsigned int Type;
	Tile_Orientation_T Orientation;
	unsigned int L1Offset;
	unsigned int L2Offset;
	unsigned int L3Offset;
	unsigned int NextPlaneUpdate;
	unsigned int Width;
	unsigned int UsedWidth;
	unsigned int Depth;
	int Overlap;
	unsigned int DimRatio;
	unsigned int Constraints;
	unsigned int FixDim[2];
	unsigned int VarDim[2];
	unsigned int Pad[4];
	unsigned int OrgPad[4];
	unsigned int ArgPad[4];
	unsigned int ItemSize;
	unsigned int MoveSize[2];
	unsigned int MoveStride;
	unsigned int Length2D[2];
	unsigned int Stride2D;
	unsigned int ArgStride;
	unsigned int BufferSize;
	unsigned int BuffCount;
	CKernel_Arg_T *CArg;
	unsigned int Index;
	unsigned char R_Event;
	unsigned char W_Event;
	unsigned char R_Channel;
	unsigned char W_Channel;
} Kernel_Arg_T;


typedef struct A_Object_T {
	NameT 	     *KerArgName;
	unsigned int Type;
	Tile_Orientation_T Orientation;
	Object_T *RefObj;
	unsigned int NextPlaneUpdate;
	unsigned int Width;
	unsigned int UsedWidth;
	unsigned int Height;
	unsigned int UsedHeight;
	unsigned int Depth;
	unsigned int Pad[4];
	unsigned int OrgPad[4];
	unsigned int ArgPad[4];
	unsigned int ArgStride;
	unsigned int BottomBuffer;
	unsigned int TopBuffer;
	unsigned int ItemSize;
	int Overlap;
	unsigned int NinPlanes;
	unsigned int NoutPlanes;
	unsigned int Alignment;
	unsigned int PreferedTileSize;
	unsigned int Constraints;
	unsigned int DimOff;
	unsigned int DimRem;
	unsigned int DimRatio;
	float        FDimRatio;
	unsigned int BuffCount;
	unsigned int BaseBuffer1;
	unsigned int BaseBuffer2;
	NameT	     *CArgName;
	union {
		unsigned int ArgN;
		Object_T *Alias;
	} Alias;
} Object_T;

typedef struct {
	NameT *Name;
	unsigned int Instance;
	unsigned int InGroup;
	InlineModeT  InlineMode;
	KernelOptimizationT KerOpt;
	Tile_Orientation_T Orientation;
	unsigned int NinPlanes;
	unsigned int NoutPlanes;
	unsigned int W;
	unsigned int H;
	unsigned int Dimension;
	unsigned int Iteration[KER_TILE_MAX];
	unsigned int RemIteration[KER_TILE_MAX];
	KernelIteratorT *IterOrder;
	unsigned int Nfor1;
	unsigned int ArgCount;
	Kernel_Arg_T **Arg;
	Kernel_Arg_T *RefArg;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	unsigned int CCallsCount;
	CKernelCall_T **CCalls;
	unsigned int BaseIndex;
	unsigned int DynSymbolCount;
	KernelDynamicSymbol_T **DynSymbol;
	unsigned int UsedL1Memory;
	unsigned int UsedL2Memory;
} Kernel_T;

typedef struct {
	NameT *Name;
	KernelCallTypeT CallType;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
	NameT *ParArgTypeName;
} KernelLib_T;

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

typedef struct {
	NameT *Name;
	unsigned int CArgCount;
	CKernel_Arg_T **CArg;
} KernelLibTemplate_T;

#define Q2F(V, N)               ((float) (((float) (V))/((1<<(N))-0)))
#define MultRndu(x,y, scale)    ((unsigned int)(((x)*(y)) + (1<<((scale)-1)))>>(scale))
#ifndef Max
#define Max(a, b)               (((a)>(b))?(a):(b))
#endif
#ifndef Min
#define Min(a, b)               (((a)<(b))?(a):(b))
#endif

/* Return aligned value, alignment is 2^Size */
#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))
extern	   KernelLibTemplate_T KernelLibTemplate[];
extern	   KernelLib_T KernelLib[];
extern int HeadKernelLib;
extern int HeadKernelLibTemplate;
extern     Kernel_T KernelStack[];
extern int HeadKernelStack;

extern     KernelGroup_T KernelGroup[];
extern int HeadKernelGroup;

extern unsigned int L1MemorySize;
extern int MaxL1TopMemory;
extern int MaxAllocatedL2;
extern char *OutputDir;
extern char *LibTemplateName;
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

#define BYTE                        1
#define HALF_WORD                   2
#define WORD                        4
#define FROM_L2                     0
#define FROM_L3                     1
#define TO_L2                       0
#define TO_L3                       1
#define NO_PADDING                  0
#define ZERO_PADDING                1
#define NO_RELU                     0
#define RELU                        1
#define NO_POOLING                  0
#define MAX_POOLING                 1
#define AVG_POOLING                 2

#endif
