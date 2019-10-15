/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef __AUTO_TILER_LIB__
#define __AUTO_TILER_LIB__

/** @defgroup groupAutoTilerApi AutoTilerAPI
@ingroup groupAutoTiler
@{ */

#include "AutoTilerLibTypes.h"

/** @defgroup CompFC CompilationFlowControl
Functions in this group control the compilation flow of the AutoTiler
@{ */

/**
 * @brief Parses AutoTiler options and performs initialization

Parses AutoTiler options and performs initialization

This function takes care of AutoTiler arguments parsing and of initialization.

Any user model should start with a call to it. it will return 0 if arguments are ok and if the initialization is sucessfull
*/
int TilerParseOptions(
	int argc,	/**< Number of arguments */
	char **argv	/**< A string for each argument */
	);

/**
@brief Generate code from the user model

Generate code from the user model

Once the model has been fully populated calling this function will produce the C code wrapping each user kernel
*/
void GenerateTilingCode();

/**
@brief Provides header file for basic kernels standard data type and a list of basic kernels header files

Provides header file for basic kernels standard data type and a list of basic kernels header files

*/
void SetUsedFilesNames(
	char *StdTypedefsName,			/**< File names where standard typedefs used by basic kernels are declared */
	unsigned int LibKernelFileCount,	/**< Number of header files for basic kernels to be imported into the current model, list of strings follows  */
	...		
	);

/**
@brief Defines files's names for code generation output

Defines files's names for code generation output

These 4 names are having as default values: GenKernelsInit.c, GenKernelsInit.h, GenKernels.c, GenKernels.h
*/
void SetGeneratedFilesNames(
	char *ArgInitsName,			/**< A .c file where compiled argument's descriptors are defined */
	char *ArgInitsHeaderName,		/**< A .h file to export compiled argument's descriptors */
	char *CallTemplatesName,		/**< A .c file for user kernels C generated code */
	char *CallTemplatesNameHeader		/**< A .h file to export user kernels C generated code */
	);

/**
@brief Defines C symbol names used for user kernel's code generation

Defines C symbol names used for user kernel's code generation

Default names are L1_Memory, L2_Memory, AllKernels and AllKernelsArgs
*/
void SetSymbolNames(
	char *L1SymbName,		/**< C var name, pointer or array, for AutoTiler dynamic needs in cluster shared L1 memory */
	char *L2SymbName,		/**< C var name, pointer or array, for AutoTiler dynamic needs in L2 memory*/
	char *KernelDescrName,		/**< C var name, array, for AutoTiler user kernel descriptors for user kernels that are not inlined */
	char *KernelDescArgName		/**< C var name, array, for AutoTiler user kernel arguments descriptors for user kernels that are not inlined */
	);

/**
@brief Set variables used for AutoTiler dynamic needs in cluster shared L1 memory and in L2 memory to be dynamic, e.g pointers, instead of arrays

Set variables used for AutoTiler dynamic needs in cluster shared L1 memory and in L2 memory to be dynamic, e.g pointers, instead of arrays
*/
void SetSymbolDynamics();

/**
@brief Set the maximum amount in shared L1 memory that the AutoTiler can use

Set the maximum amount in shared L1 memory that the AutoTiler can use
*/
void SetL1MemorySize(
	unsigned int Size	/**< Maximum size in byte */
	);

/**
@brief Control code generation inline strategy

Control code generation inline strategy

User kernels can be either always inlined, never inlined or inlined only if there is a single instance of a given user kernel
*/
void SetInlineMode(
	InlineModeT Mode	/**< Inline mode */
	);

/**
@brief Control user kernel optimizations

Control user kernel optimizations

Previous optimization settings is changed based on On and Off parameters.
Initial value is On for all optimizations.
*/
void SetKernelOpts(
	KernelOptimizationT On,	/**< Optimizations to be turned ON */
	KernelOptimizationT Off /**< Optimizations to be turned OFF */
	);

/** @} */ // End of CompilationFlowControl group

/* To create the model */
/* Adding one basic kernel template */

/** @defgroup BasicK BasicKernelTemplate
Functions in this group should be used to add basic kernels templates
@{ */
/**
@brief Adds a new basic kernel type template

Adds a new basic kernel template
*/
void LibKernelTemplate(
	char *TemplateName,		/**< The template name */
	CKernel_Arg_T **CArgs		/**< List of C typed arguments provided by CArgs()  */
	);

/**
@brief Returns Template type for TemplateName

Returns Template type for TemplateName
*/
CKernel_Arg_T **KerTypeTemplate(
	char *TemplateName		/**< The template name */
	);


/**
@brief Adds a new basic kernel template

Adds a new basic kernel template
*/
void LibKernel(
	char *KernelName,		/**< The basic kernel name */
	KernelCallTypeT CallType,	/**< Sequential or Parallel */
	CKernel_Arg_T **CArgs,		/**< List of C typed arguments provided by CArgs()  */
	char *ParArgTypeName		/**< struct typedef name to group arguments in a single struct for parallel calls */
	);

/** @} */ // End of BasicK group

/* User kernel C template */

/** @defgroup UserK UserKernel
Functions in this group should be used to add user kernels
@{ */

/** @defgroup IterSpaceK KernelIterationSpace
Functions in this group should be used to document the dimension and characteristics of the user kernel iteration space
@{
*/

/**
@brief Define iteration space dimension

Define iteration space dimension
*/
KernelDimensionT *KernelDimensions(
	unsigned int InPlanes,		/**< Number of input planes */
	unsigned int W,			/**< Data plane width */
	unsigned int H,			/**< Data plane height */
	unsigned int OutPlanes		/**< Number of output planes */
	);


/**
@brief Define iteration space dimension and user defined symbols

Define iteration space dimension and user defined symbols
*/
KernelDimensionT *KernelDimensionsAndUserSymbols(
	unsigned int InPlanes,			/**< Number of input planes */
	unsigned int W,				/**< Data plane width */
	unsigned int H,				/**< Data plane height */
	unsigned int OutPlanes,			/**< Number of output planes */
	KernelDynamicSymbol_T **UserSymbols	/**< List of user defined symbols */
	);

/**
@brief Define one user defined symbol with associated value.

Define one user defined symbol with associated value.
*/
KernelDynamicSymbol_T *S_Dyn(
	char *Name,	/**< Name of the user defined symbol */
	int Value	/**< Value of the user defined symbol */
	);

/**
@brief Define user defined symbols with associated value. Used in non inlined code generation.

Define user defined symbols with associated value. Used in non inlined code generation.
*/
KernelDynamicSymbol_T **KerDynamicSymbols(
	int SCount,	/**< Number of pairs (SymbolName, SymbolValue) */
	...);



/**
@brief Define In and Out planes dimension

Define In and Out planes dimension. Since W and H are defined for each user kernel argument this variant is a shortcut of KernelDimensions()
*/
KernelDimensionT *KernelOuterDimensions(
	unsigned int InPlanes,	/**< Number of input planes */
	unsigned int OutPlanes  /**< Number of output planes */
	);

/**
@brief Define the number of iteration spaces and and how they are nested

Define the number of iteration spaces and and how they are nested

@param IterDim Number of nested iteration levels
@param ... Ordered list of nested iteration spaces, iterators are defined by enum KernelIteratorT
@see KernelIteratorT for defined iteration spaces
@code
// Iteration space is Out Planes first then inner data planes then In data planes
KernelIterationOrder(3, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE)
@endcode
*/
KernelIterationT *KernelIterationOrder(
	KernelIterationDimT IterDim,
	...				
	);
/** @} */ // End of IterSpaceK group

/** @defgroup CArgs UserKernelCArguments
Functions in this group should be used to add C arguments to a user kernel
@{ */
/**
@brief Creates a list of ArgCount C arguments, each C argument is created by TCArg()

Creates a list of ArgCount C arguments, each C argument is created by TCArg()

@param ArgCount Number of C arguments
@param ... List of TCArg() calls
@code
// 2 C arguments, Par0 of type int and Par1 of type char *
CArgs(2, TCArg("int", "Par0", "char *", "Par1"));
@endcode
*/
CKernel_Arg_T **CArgs(
	unsigned int ArgCount,
	...
	);

/**
@brief Creates a typed C argument

Creates a typed C argument
*/
CKernel_Arg_T *TCArg(
	char *ArgType,			/**< C argument type as a string */
	char *ArgName			/**< C argument name as a string */
	);
/** @} */ // End of CArgs group

/* Basic kernels calls */
/** @defgroup Calls KernelCalls
Functions in this group should be used to add calls to basic kernels in a user kernel or user kernels in a user kernel group
@{
*/
/**
@brief Creates a list of calls to basic or user kernels

Creates a list of calls to basic or user kernels

@param CallCount Number of calls
@param ... A list of Call() calls
*/
CKernelCall_T **Calls(
	unsigned int CallCount,
	...
	);

/**
@brief Create a call to a basic or a user kernel

Create a call to a basic or a user kernel
*/
CKernelCall_T *Call(
	char *CallName,			/**< Name of the function the be called, a basic kernel or a user kernel if in a group */
	KernelCallLocationT CallLocation, /**< Where to insert the call into the iteration structure */
	ArgBindingDescr_T **BindingList /**< List of bindings for callee's arguments: Bindings(), Bindings(), ... */
	);

/* Binding basic kernel arguments to user kernel C template of kernel arguments */
/** @defgroup ArgBind ArgumentBinding
Functions in this group should be used to specify how a given basic kernel call argument is bound
@{ */

/**
@brief Create a list of bindings between an argument and an actual value

Create a list of bindings between an argument and an actual value

@param BCount Number of bindings
@param ... List of bindings: C_Arg(), C_ArgIndex(), C_ArgPlusImmOffset(), C_ArgPlusCvarOffset(), C_ArgIndirect(), Imm(), K_Arg()
*/
ArgBindingDescr_T **Bindings(
	int BCount,			/* Number of bindings */
	...				/* List of bindings: C_Arg(), C_ArgIndex(), C_ArgPlusImmOffset(), C_ArgPlusCvarOffset(), C_ArgIndirect(), Imm(), K_Arg() */
	);

/**
@brief Binds argument to a C user kernel or user kernel group

Binds argument to a C user kernel or user kernel group
*/
ArgBindingDescr_T *C_Arg(
	char *ArgName			/**< A C argument name, usually a C argument of the user kernel */
	);

/**
@brief Binds argument to a C user kernel or user kernel group argument and adds an immediate Offset

Binds argument to a C user kernel or user kernel group argument and adds an immediate Offset.

	In case ArgName is a pointer Offset is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ArgPlusImmOffset(
	char *ArgName,			/**< A C argument name, usually a C argument of the user kernel */
	int Offset			/**< An integer offset to be added to ArgName */
	);

/**
@brief Binds argument to a C user kernel or user kernel group argument and adds an Offset which is another C argument

Binds argument to a C user kernel or user kernel group argument and adds an Offset which is another C argument.

	In case ArgName is a pointer Offset is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ArgPlusCvarOffset(
	char *ArgName,			/**< A C argument name, usually a C argument of the user kernel */
	char *Cvar			/**< A C argument name, usually a C argument of the user kernel */
	);

/**
@brief Binds argument to a C user kernel or user kernel group argument and adds the current plane index given by Sel multiplied by Mult.

Binds argument to a C user kernel or user kernel group argument and adds the current plane index given by Sel multiplied by Mult.

	In case ArgName is a pointer plane index corresponding to Sel is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ArgPlusPlaneOffset(
	char *ArgName,			/**< A C argument name, usually a C argument of the user kernel */
	KernelIteratorT Sel,	 	/**< Which index:
                                                KER_TILE: the current tile index in the data plane,
                                                KER_IN_PLANE: the in plane current index
                                                KER_OUT_PLANE: the out plane current index */
	int Mult			/**< A multiplication factor to be applied to selected index */
	);


/**
@brief Binds argument to a C user kernel or user kernel group subscripted by an Index.

Binds argument to a C user kernel or user kernel group subscripted by an Index.

	ArgName should be of pointer type and the subscript index is taken from one of the iterators.
	The binding scheme is ArgName[Index(Sel)*Mult].
	Index(Sel) is the current value of the iteration counter of the selected iteration space.
*/
ArgBindingDescr_T *C_ArgIndex(
	char *ArgName,			/**< A C kernel argument, expected to be of pointer type */
	KernelIteratorT Sel,		/**< Which index:
						KER_TILE: the current tile index in the data plane,
						KER_IN_PLANE: the in plane current index
						KER_OUT_PLANE: the out plane current index */
	int Mult			/**< A multiplication factor to be applied to selected index */
	);

/**
@brief Binds argument to a C user kernel or user kernel group subscripted by an Offset

Binds argument to a C user kernel or user kernel group subscripted by an Offset

	ArgName should be of pointer type and Offset undergoes type conversion
	The binding scheme is *(ArgName+Offset)
*/
ArgBindingDescr_T *C_ArgIndirect(
	char *ArgName,			/**< A user kernel C argument content: *Arg  C argument is expected to be a pointer */
	int Offset			/**< If not null use offset as *(Arg+Offset) */
	);

/**
@brief Binds argument to an immediate integer value.

Binds argument to an immediate integer value.
*/
ArgBindingDescr_T *Imm(
	int Value			/**< An immediate value */
	);

/**
@brief Binds argument to a user defined symbol's associated value

Binds argument to a user defined symbol's associated value
*/
ArgBindingDescr_T *UserSymb(
	char *Name			/**< A user symbol name */
	);

/**
@brief Binds argument to one of the user kernel dimension InPlane/OutPlane/Width/Height

Binds argument to one of the user kernel dimension InPlane/OutPlane/Width/Height
*/
ArgBindingDescr_T *KerDim(
	KernelDimT Dim			/**< User kernel dimension: K_INP, K_OUTP, K_W, K_H */
	);


/**
@brief Binds argument to a user kernel argument (a tiled argument).

Binds argument to a user kernel argument (a tiled argument).

Argument selection can be:

	KER_ARG_TILE:		Pointer to the current tile.
       	KER_ARG_TILE_OFF	Pointer to the current tile plus byte offset.
       	KER_ARG_TILE_1_2	Pointer to the current tile plus 1/2 of tile size offset.
       	KER_ARG_TILE_W		Width of the current tile.
       	KER_ARG_TILE_H		Height of the current tile.
       	KER_ARG_TILE_W0		Standard width of the tile (last tile excluded).
       	KER_ARG_TILE_H0		Standard height of the tile (last tile excluded).
       	KER_ARG			Pointer to the data plane.
       	KER_ARG_W		Width of the data plane.
       	KER_ARG_H		Height of the data plane.
       	KER_ARG_NTILES		Number of tiles in the data plane.
       	KER_ARG_TILEINDEX	Index of the current tile, starts at 0.
*/
ArgBindingDescr_T *K_Arg(
	char *ArgName,			/**< A tiled user kernel argument name */
	KernelArgSelect_T ArgSelect	/**< Select which user kernel argument property to be used */
	);

/**
@brief Binds argument to a user kernel argument (a tiled argument) and combine it with Value using Oper

Binds argument to a user kernel argument (a tiled argument) and combine it with Value using Oper

See K_Arg() for ArgName and ArgSelect. Supported operation are defined in ArgBindingOper. Value is a signed immediate constant.
*/
ArgBindingDescr_T *K_ArgOper(
	char *ArgName,			/**< A tiled user kernel argument name */
	KernelArgSelect_T ArgSelect,	/**< Select which user kernel argument property to be used */
	char Oper,			/**< Operation, see ArgBindingOper. Valid: + - * / %  */
	int Value			/**< A signed immediate value */
	);

/** @} */ // End of ArgBind group
/** @} */ // End of Calls group

/* User kernel arguments */
/** @defgroup UserKArg UserKernelArguments
Functions in this group should be used to capture user kernel arguments
@{ */

/**
@brief Creates a list of user kernel arguments.

Creates a list of user kernel arguments.

@param KerArgCount Number of user kernel arguments
@param ...  List of user kernel arguments: KerArg() or KerArgPad(), KerArg(), ...
*/
Object_T **KerArgs(
	unsigned int KerArgCount,
	...			
	);

/**
@brief Creates one user kernel argument.

Creates one user kernel argument.
*/
Object_T *KerArg(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one user kernel argument.  Kernel width/height is strided.

Creates one user kernel argument.
*/
Object_T *KerArg2D(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Stride,		/**< Kernel argument W is strided with stride factor = Stride in bytes */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one 3 dimensional user kernel argument.

Creates one 3 dimensional user kernel argument.
*/
Object_T *KerArg3D(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Depth,		/**< Kernel argument depth, e.g 3rd dimension */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one user kernel argument with fixed padding before and after the tile generated for this argument.

Creates one user kernel argument with fixed padding before and after the tile generated for this argument.

	This is generally used for kernels requiring a delay line like in a FIR filter.
*/
Object_T *KerArgPad(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int BottomBuffer,	/**< Amount of buffer to be added before this kernel argument, unit is ItemSize */
	unsigned int TopBuffer,		/**< Amount of buffer to be added after this kernel argument, unit is ItemSize */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one user kernel argument with fixed padding left/right and top/bottom for this argument.

Creates one user kernel argument with fixed padding left/right and top/bottom for this argument.

*/
Object_T *KerArgP(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int PadL,		/**< Amount of padding to be added on the left of a line, unit is Item */
	unsigned int PadR,		/**< Amount of padding to be added on the the right of a line, unit is Item */
	unsigned int PadT,		/**< Amount of padding to be added on the top of a column, unit is Item */
	unsigned int PadB,		/**< Amount of padding to be added on the bottom of a column, unit is Item */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one user kernel argument with fixed padding left/right and top/bottom for this argument. Kernel width/height is strided

Creates one user kernel argument with fixed padding left/right and top/bottom for this argument.

*/
Object_T *KerArg2DP(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Stride,		/**< Kernel argument W is strided with stride factor = Stride in bytes */
	unsigned int PadL,		/**< Amount of padding to be added on the left of a line, unit is Item */
	unsigned int PadR,		/**< Amount of padding to be added on the right of a line, unit is Item */
	unsigned int PadT,		/**< Amount of padding to be added on the top of a column, unit is Item */
	unsigned int PadB,		/**< Amount of padding to be added on the bottom of a column, unit is Item */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one 3 dimensional user kernel argument with fixed padding left/right and top/bottom for this argument.

Creates one 3 dimensional user kernel argument with fixed padding left/right and top/bottom for this argument.

*/
Object_T *KerArg3DP(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Depth,		/**< Kernel argument depth, e.g 3rd dimension */
	unsigned int PadL,		/**< Amount of padding to be added on the left of a line, unit is Item */
	unsigned int PadR,		/**< Amount of padding to be added on the right of a line, unit is Item */
	unsigned int PadT,		/**< Amount of padding to be added on the top of a column, unit is Item */
	unsigned int PadB,		/**< Amount of padding to be added on the bottom of a column, unit is Item */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);

/**
@brief Creates one user kernel argument with dimension [WxH] but used partially as [UsedWxUsedH]

Creates one user kernel argument with dimension [WxH] but used partially as [UsedWxUsedH]

	This is generally used for kernels having vertically tiled arguments that are consumming only a subset of the whole
data plane. In the horizontal case it is not an issue since H can be safely reduced because arrays have line major layout but
for vertical we need to be aware that the used width is in fact in a data planne that is wider.
*/
Object_T *KerArgPart(
	char 	     *KerArgName,	/**< Kernel argument name */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int UsedW,		/**< Kernel argument Data plane effective width, has to be < W */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int UsedH,		/**< Kernel argument Data plane effective height, has to be < H */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	unsigned int TileOverlap,	/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName,			/**< To which user kernel C argument this kernel argument is related to */
	unsigned int NextPlaneUpdate	/**< When moving to the next plane use NextPlaneUpdate as a multiplication factor */
	);


/** @} */ // End of UserKArg group

/* User kernel */
/**
@brief Creates a user kernel

Creates a user kernel
*/
Kernel_T *UserKernel(
	char *TemplateName,		/**< C name to be used for this user kernel */
	KernelDimensionT *KerDim,	/**< See KernelDimensions() */
	KernelIterationT *KerIter,	/**< See KernelIterationOrder() */
	Tile_Orientation_T Orientation,	/**< Kernel tiling orientation */
	CKernel_Arg_T **CArg,		/**< See Carg() and TCArg() */
	CKernelCall_T **CCalls,		/**< See Calls() and Call() */
	Object_T **KerArg		/**< See KerArgs() and KerArg(), KerArgPad() */
	);

/** @} */ // End of UserK group

/** @defgroup KernelGroup UserKernelGroup
Functions in this group should be used to create groups of user kernels

A user kernel group starts with an OpenKernelGroup() followed by multiple UserKernel() and CloseKernelGroup()

Then the group header and how user kernels are called in the group are captured in UserKernelGroup()
@{ */
/* To open a kernel group */

/**
@brief Open a user kernel group

Open a user kernel group
*/
void OpenKernelGroup(
	char *GroupName 		/**< C name to be used for the group */
	);

/* To close a kernel group */
/**
@brief Close currently open user kernel group.

Close currently open user kernel group.
*/
void CloseKernelGroup();


/* To add a call to a user kernel in a kernel group */
/**
@brief Call a user kernel in a user kernel group

Call a user kernel in a user kernel group
*/
CKernelCall_T *UserKernelCall(
	char *CallName,			/**< A user kernel name */
	KernelCallLocationT CallLocation, /**< Here only LOC_IN_GROUP is valid */
	ArgBindingDescr_T **BindingList /**< See Bindings(). restricted to C_Arg(), C_ArgIndex(), C_ArgIndirect() and Imm() */
	);

/* A user kernel group */
/**
@brief Declare a user kernel group

Declare a user kernel group
*/
void UserKernelGroup(char *GroupName,	/**< Group's name as in OpenKernelGroup() */
       	CKernel_Arg_T **CArg,		/**< See Carg() and TCArg() */
       	CKernelCall_T **CCalls		/**< See Calls() and UserKernelCall() */
	);
/** @} */ // End of UserKernelGroup group

/* Book keeping */
/** @defgroup BookKeeping Miscelanous
Service Functions
@{ */

/**
@brief Return the concatenation of 2 names.

Return the concatenation of 2 names.

This is usefull to create names derivatives when writing generators.
*/
char *AppendNames(
	char *Name1,	/**< First name passed as a C string */
	char *Name2	/**< Second name passed as a C string */
     );

/**
@brief Generates an error message and aborts execution

Generates an error message and aborts execution

@param Message A format string similar to the one used in C printf()
@param ... Arguments used by the format sting in Message
*/
void GenTilingError(
	char *Message,
	...
	);

/**
@brief Generates a warning message. Does not abort.

Generates a warning message. Does not abort.

@param Message A format string similar to the one used in C printf()
@param ... Arguments used by the format sting in Message
*/
void GenTilingWarning(
	const char *Message,
	...
	);

/** @} */ // End of BookKeeping group

/** @} */ // End of add to group groupAutoTilerApi

extern ArgBindingDescr_T *AT_NO_ARG_BINDING, *AT_IGNORE_ARG_BINDING;
extern CKernelCall_T *AT_NO_CALL;
extern Object_T *AT_NO_KER_ARG;

extern void SetAT_TestFile(char *AT_TestFileName);
extern void AT_PrepareForTest(char *Name,
                    v4s DataSize, // int In_DataSize, int Filter_DataSize, int Bias_DataSize, int Out_DataSize,
                    v4s L3,
                    int InFeat, int OutFeat, int Width, int Height,
                    int FSc, int ConvStride, v4s PadInp, int ConvDoReLU, int DoConv,
                    int FSp, int PoolStride, v4s PadInc, int PoolDoReLU, int DoPool,
                    int DoLinear,
                    int DoReLU,
                    int DoSoftMax,
                    int Norm,
                    int NormBias);
extern void AT_TestFinalize();

#endif
