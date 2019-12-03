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

#ifndef __AUTO_TILER_LIB__
#define __AUTO_TILER_LIB__

#include <stdio.h>

/** @defgroup groupAutoTilerApi AutoTilerAPI
@ingroup groupAutoTiler
@{ */

#include "AutoTilerLibTypes.h"

/** @defgroup CompFC CompilationFlowControl
Functions in this group control the compilation flow of the AutoTiler
@{ */

/**
@brief Macros to pass value to AT_SetGraphCtrl and to CNN_SetGenCtrl/CNN_SetGenCtrlList

Macros to pass value to AT_SetGraphCtrl and to CNN_SetGenCtrl/CNN_SetGenCtrlList
*/
#define AT_OPT_ON       ((void *) 1)
#define AT_OPT_OFF      ((void *) 0)
#define AT_OPT_VAL(Val) ((void *) (Val))

/**
@brief Set user kernel style modelization mode.

Set user kernel style modelization mode. If Old=1 uses Kernel Arg property to express Ker Arg iter space, If Old=0 use KerArgSpace() extra argument instead.
*/
void SetStyle(
	int Old		/**< Style mode, 0: New style, != 0 Old style */
	);

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

These 2 names are having as default values: GenKernels.c, GenKernels.h
*/
void SetGeneratedFilesNames(
	char *CallTemplatesName,		/**< A .c file for user kernels C generated code */
	char *CallTemplatesNameHeader		/**< A .h file to export user kernels C generated code */
	);

/**
@brief Defines C symbol names used for user kernel's code generation

Defines C symbol names used for user kernel's code generation

Default names are L1_Memory, L2_Memory
*/
void SetSymbolNames(
	char *L1SymbName,		/**< C var name, pointer or array, for AutoTiler dynamic needs in cluster shared L1 memory */
	char *L2SymbName 		/**< C var name, pointer or array, for AutoTiler dynamic needs in L2 memory*/
	);

/**
@brief Defines C symbol names used for user kernel's code generation

Defines C symbol names used for user kernel's code generation

Default names are L1_Memory, L2_Memory, L3_Memory
*/
void SetMemoryBaseNames(
	char *L1SymbName,		/**< C var name, pointer or array, for AutoTiler dynamic needs in cluster shared L1 memory */
	char *L2SymbName,		/**< C var name, pointer or array, for AutoTiler dynamic needs in L2 memory*/
	char *L3SymbName 		/**< C var name, pointer or array, for AutoTiler dynamic needs in L3 memory (PreferedExecLoc) */
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
@brief Set the maximum amount in shared L1 memory, L2 internal memory and L3 external RAM that the AutoTiler can use

Set the maximum amount in shared L1 memory, L2 internal memory and L3 external RAM that the AutoTiler can use
*/
void SetMemorySizes(
	unsigned int L1Size,	/**< Maximum L1 size in byte */
	unsigned int L2Size,	/**< Maximum L2 size in byte */
	unsigned int L3Size	/**< Maximum L3 size in byte */
	);

/**
@brief Configure memory devices

Configure memory devices, A list of count items where each item is a tuple of 5 elements
	Space  			AT_MemLocation_T  	Which memory space
	AvailableMemory 	int			available memory to be used on this device
	MemmoryBaseName 	char *			a legal C var name to be used as a base in this memory device
	ConstFileName 		char *			a file name to be used if loading constant in this device is needed, flash device only
	ExtManaged		int			0 if device configuration code should be generated, != 0 if managed externally
*/
void SetMemoryDeviceInfos(
	int Count,		/**< Number of Items */
	...			/* Item list */
	);

/* List of Count items,
        Item: ATMemory Space AvailableMemory, "MemmoryBaseName", "ConstFileName", ExtManaged
*/

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

/**
@brief Provides a folder name where to find constant files passed to ConstInfo()

Provides a folder name where to find constant files passed to ConstInfo()
*/
void SetConstDir(
	char *DirName		/** A valid directory name */
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
	char *ParArgTypeName,		/**< struct typedef name to group arguments in a single struct for parallel calls */
	CNN_LayerOp_T *CNN_Match	/**< Matching CNN kernels */
	);
/**
@brief Find best mapping for the group KerOper1,KerOper2 in the existing Kernel library

Find best mapping for the group KerOper1,KerOper2 in the existing Kernel library
*/
KernelLib_T *KerLibMatch(
        KernelOper_T KerOper1,		/**< Symbolic operation */
	KernelOper_T KerOper2,		/**< Optional symbolic operation grouped with KerOper1 */
        int ParallelFeatures,		/**< If 0 one feature is evaluated by several cores, if not each core evaluates a different feature */
        int I1_Type,			/**< Input1 data size in bytes or 0 if to be ignored */
	int I2_Type,			/**< Input2 data size in bytes or 0 if to be ignored */
	int I3_Type,			/**< Input3 data size in bytes or 0 if to be ignored */
	int I4_Type,			/**< Input4 data size in bytes or 0 if to be ignored */
	int O_Type,			/**< Output data size in bytes or 0 if to be ignored */
        int Fx,				/**< Filter X dimension */
	int Fy,				/**< Filter Y dimension */
	int Dx,				/**< Filter dilation in X dimension */
	int Dy,				/**< Filter dilation in Y dimension */
	int Sx,				/**< Filter stride in X dimension */
	int Sy				/**< Filter stride in Y dimension */
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
@brief Define user defined symbols with associated value. Used in non inlined code generation. Returns NULL.

Define user defined symbols with associated value. Used in non inlined code generation. Returns NULL.
*/
void UserSymbols(
	int SCount,	/**< Number of pairs (SymbolName, SymbolValue) */
	...);


/* Internal routine to initialize kernel iteration dimensions names and related variables
	Old style:
		0: Tile0, Tile1, Tile2, D0, D1, D2, D3
		1: Tile0, Tile1, Tile2, InPlane, OutPlane, D2, D3
*/

void InitKernelIterInfos(
	int OldStyle
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


/**
@brief Define one iteration dimension of the current kernel iteration space, this dimension must not be a tiled one. Actual dimension value is provided.

Define one iteration dimension of the current kernel iteration space, this dimension must not be a tiled one. Actual dimension value is provided.
*/
KernelIteratorDescrT *IterFixedSpace(
	KernelIteratorT IterSpace,	/**< Which iteration dimension of the kernel iteration space, must be a non tiled iteration space */
	unsigned int Dim		/**< Actual value for the given dimension, must be > 0 */
	);

/**
@brief Define one iteration dimension of the current kernel iteration space, dimension must not be a tiled one. Actual dimension value is provided and subject to division.

Define one iteration dimension of the current kernel iteration space, dimension must not be a tiled one. Actual dimension value is provided and subject to division
using PrefDiv as a prefered divider or a multiple of the divider.
*/
KernelIteratorDescrT *IterParSpace(
	KernelIteratorT IterSpace,	/**< Which iteration dimension of the kernel iteration space, must be a non tiled iteration space */
	unsigned int Dim,		/**< Actual value for the given dimension, must be > 0 */
	unsigned int PrefDiv		/**< Prefered space Dim divider */
	);

/**
@brief Define one iteration dimension of the current kernel iteration space, this dimension must be a tiled one. The autotiler will figure out the actual dimension.

Define one iteration dimension of the current kernel iteration space, this dimension must not be a tiled one. The autotiler will figure out the actual dimension.
*/
KernelIteratorDescrT *IterTiledSpace(
	KernelIteratorT IterSpace	/**< Which iteration dimension of the kernel iteration space, must be a tiled iteration space */
	);


/**
@brief Define the number of iteration spaces and and how they are nested

Define the number of iteration spaces and and how they are nested

@param Dim Number of nested iteration levels
@param ... Ordered list of nested iteration spaces, iterators are defined by enum KernelIteratorT
@see KernelIteratorDescrT for a description of an iteration space
@see IterFixedSpace() and IterTiledSpace() for a description of how to create an iteration space
@code
// User Kernel's Iteration space is outer dim: ITER_D0 (non tiled, actual dim = 10), then TILE0 (a tiled dimension) and then ITER_D1 (non tiled, actual dim=20)
KernelIterSpace(3, IterFixedSpace(KER_ITER_D0, 10), IterTileSpace(KER_ITER_TILE0), IterFixedSpace(KER_ITER_D1, 20)
@endcode
*/
KernelIterationSpaceT *KernelIterSpace(
	unsigned int Dim,		/**< Number of dimension for the iteration space */
	...				/**< List of KernelIteratorDescrT *, one for each dimension of the user kernel iteration space, IterFixedSpace() or IterTiledSpace() */
	);

/**
@brief Creates a list of ArgCount Kernel C arguments

Creates a list of ArgCount Kernel C arguments
*/
CKernel_Arg_T **AllocateCArgs(
	unsigned int ArgCount		/**< Number of C Arguments */
	);

/**
@brief Creates a list of ArgCount Kernel C arguments, then list of TCArg(), one for each Argument

Creates a list of ArgCount Kernel C arguments, then list of TCArg(), one for each Argument
*/
CKernel_Arg_T **CArgs(
	unsigned int ArgCount,		/**< Number of C Arguments */
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


/**
@brief Creates a descriptor for an initialized constant vector

Creates a descriptor for an initialized constant vector
*/
ConstInit_T *ConstInfo(
	char *FileName,			/**< Name of the file containing the list of values for this constant vector */
	int Format,			/**< Format of the list of value, 0: floating point, 1: fixed point */
	int Binary,			/**< If 1 file content is binary, if 0 file content is text */
	int Size,			/**< Size of the fixed point container */
	int Fract			/**< Fractional part size, Qx.Fract, note that x+Fract < Size */
	);

/**
@brief Creates a typed C argument with location info

Creates a typed C argument with location info
*/
CKernel_Arg_T *TCArgInfo(
	char *ArgType,			/**< C argument type as a string */
	char *ArgName,			/**< C argument name as a string */
	ArgScope_T Scope,		/**< Argument scope, ARG_SCOPE_ARG: passed as an argument, ARG_SCOPE_GLOBAL: global symbol */
	ArgDirection_T Dir,		/**< Argument's direction (in, out or in out) */
	AT_MemLocation_T HomeLoc,	/**< Permanent location in the mem hierarch variants of L3 or L2 */
	AT_MemLocation_T ExecLoc,	/**< Location of the argument when it is used, can be != HomeLoc */
	ConstInit_T *ConstInit		/**< Descriptor for initialized constant */
	);

/**
@brief Creates a typed C argument with location info, symbol considered to be allocated externally by the user

Creates a typed C argument with location info, symbol considered to be allocated externally by the user
*/
CKernel_Arg_T *TCArgInfoA(
	char *ArgType,			/**< C argument type as a string */
	char *ArgName,			/**< C argument name as a string */
	ArgScope_T Scope,		/**< Argument scope, ARG_SCOPE_ARG: passed as an argument, ARG_SCOPE_GLOBAL: global symbol */
	ArgDirection_T Dir,		/**< Argument's direction (in, out or in out) */
	AT_MemLocation_T HomeLoc,	/**< Permanent location in the mem hierarch variants of L3 or L2 */
	AT_MemLocation_T ExecLoc,	/**< Location of the argument when it is used, can be != HomeLoc */
	ConstInit_T *ConstInit		/**< Descriptor for initialized constant */
	);

/**
@brief Creates a C argument location info only

Creates a C argument location info only
*/
CArg_Descriptor_T *ArgInfo(
	ArgScope_T Scope,		/**< Argument scope, ARG_SCOPE_ARG: passed as an argument, ARG_SCOPE_GLOBAL: global symbol */
	ArgDirection_T Dir,		/**< Argument's direction (in, out or in out) */
	AT_MemLocation_T HomeLoc,	/**< Permanent location in the mem hierarch variants of L3 or L2 */
	AT_MemLocation_T ExecLoc,	/**< Location of the argument when it is used, can be != HomeLoc */
	ConstInit_T *ConstInit		/**< Descriptor for initialized constant */
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
@brief Prepares a list of CallCount calls 

Prepares a list of CallCount calls 

@param CallCount Number of calls
*/
CKernelCall_T **AllocateCalls(
	unsigned int CallCount
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
@brief Binds argument to a C user kernel or user kernel group argument and combines it with an immediate Value

Binds argument to a C user kernel or user kernel group argument and combines it with an immediate Value

	Oper can be +,-,*,/,%
	In case ArgName is a pointer Offset is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ArgImmOper(
	char *ArgName,			/**< A C argument name, usually a C argument of the user kernel */
	char Oper,			/**< The operation to be performed passed as a char, can be +,-,*,/,% */
	int Value			/**< An integer value to be combined with ArgName using Oper */
	);

/**
@brief Binds argument to an immediate value combined to a C user kernel or user kernel group argument through oper

Binds argument to an immediate value combined to a C user kernel or user kernel group argument through oper

	Oper can be +,-,*,/,%,<<,>>
	In case ArgName is a pointer Offset is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ImmArgOper(
	int Value,			/**< An integer value to be combined with ArgName using Oper */
	char *Oper,			/**< The operation to be performed passed as a char, can be +,-,*,/,%,<<,>> */
	char *ArgName			/**< A C argument name, usually a C argument of the user kernel */
	);


/**
@brief Binds argument to a C user kernel or user kernel group argument and combines it with a C variable

Binds argument to a C user kernel or user kernel group argument and combines it with an C variable

	Oper can be +,-,*,/,%
	In case ArgName is a pointer Offset is subject to usual C pointer type promotion.
*/
ArgBindingDescr_T *C_ArgCOper(
	char *ArgName,			/**< A C argument name, usually a C argument of the user kernel */
	char Oper,			/**< The operation to be performed passed as a char, can be +,-,*,/,% */
	char *Cvar			/**< Name of the C variable used in the right hqnd side part of the expressionn */
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
@brief Binds argument to one of the user kernel iterator actual value

Binds argument to one of the user kernel iterator actual value
*/
ArgBindingDescr_T *Ker_IteratorIndex(
	KernelIteratorT Sel		/**< Selected iteration space */
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
@brief Binds argument to one of the parametric sub space of a user kernel argument.

Binds argument to one of the parametric sub space of a user kernel argument.

Argument selection can be:
	KER_ARG_PARTILE_DIM	Actual dimension of the parametric space after tiling
	KER_ARG_PARTILE_SIZE	Dimension of one tile of the parametric space after tiling
*/
ArgBindingDescr_T *K_ArgPar(
	char *ArgName,			/**< A tiled user kernel argument name */
	KernelArgSelect_T ArgSelect,	/**< Select which user kernel argument property to be used */
	KernelIteratorT ItSpace		/** Which Iteration space, has to a parametric one */
	);

/**
@brief Binds argument to a user kernel argument (a tiled argument) and combine it with Value using Oper

Binds argument to a user kernel argument (a tiled argument) and combine it with Value using Oper

See K_Arg() for ArgName and ArgSelect. Supported operation are defined in ArgBindingOper. Value is a signed immediate constant.
*/
ArgBindingDescr_T *K_ArgOper(
	char *ArgName,			/**< A tiled user kernel argument name */
	KernelIteratorT KerIter, 	/**< Select kernel iteration space */
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
@brief Creates a tile dimension constraint for a kernel argument

Creates a tile dimension constraint for a kernel argument, Tile dimension T must be equal to K * Div + Rem, Rem can be greater than Div

@param Div Division factor
@param Rem Constant term to be added to a multiple of Div to obtain tile dim
*/
unsigned int MkTCons(
	unsigned int Div,
	unsigned int Rem);

/**
@brief Creates a list of user kernel arguments.

Creates a list of user kernel arguments.

@param KerArgCount Number of user kernel arguments
*/
Object_T **AllocateKerArgs(
	unsigned int KerArgCount	/**< Number of Kernel arguments */
	);

/**
@brief Creates a list of user kernel arguments.

Creates a list of user kernel arguments.

@param KerArgCount Number of user kernel arguments
@param ...  List of user kernel arguments: KerArg() or KerArgPad(), KerArg(), ...
*/
Object_T **KerArgs(
	unsigned int KerArgCount,	/**< Number of Kernel arguments */
	...
	);

/**
@brief Define Kernel argument's iteration space.

Define Kernel argument's iteration space. Each provided dimension must also be defined in the User Kernel's iteration space, the most inner dimension must be a tiled dimension.
*/
KernelArgDimDescrT *KerArgSpace(
	unsigned int Dim,		/**< Number of dimensions for this Kernel argument */
	...				/**< List of KernelIteratorT */
	);

#ifdef LEGACY
Object_T *_KerArgP(
	char *KerArgName,			/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,	/**< Kernel argument space descriptor */
	Object_Type_T ObjType,			/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,				/**< Kernel argument width */
	unsigned int H,				/**< Kernel argument height */
	v4s PadTile,				/**< Kernel argument Padding [Left,Right,Top,Bottom], Used for tiling, = PadExec if no cascaded calls, >= PadExec else */
	v4s PadExec,				/**< Kernel argument Padding [Left,Right,Top,Bottom], Used at exec time */
	unsigned int ItemSize,			/**< Kernel argument data type size in bytes */
	int TileOverlap,			/**< Kernel argument overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint,	/**< Kernel argument constraints */
	unsigned int PreferedTileSize,		/**< Kernel argument prefered tile size, expressed on tile size - overlap, tile size must be a multiple of PreferedTileSize */
	char *CArgName);			/**< To which user kernel C argument this kernel argument is related to */
#endif

/**
@brief Creates one user kernel argument. Kernel argument Space is explicitely described

Creates one user kernel argument. Kernel argument Space is explicitely described
*/
Object_T *KerArg(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);

Object_T *KerArgP(
	char *KerArgName,
	KernelArgDimDescrT *KerArgSpace,
	Object_Type_T ObjType,
	unsigned int W,
	unsigned int H,
	unsigned int UsedW,
	unsigned int UsedH,
	v4s PadTile,
	v4s PadExec,
	unsigned int ItemSize,
        int TileOverlap,
	KernelArgConstraints_T Constraint,
        unsigned int PreferedTileSize,
	char *CArgName);


/**
@brief Creates one user kernel argument with fixed padding before and after the tile generated for this argument. Kernel argument Space is explicitely described

Creates one user kernel argument with fixed padding before and after the tile generated for this argument. Kernel argument Space is explicitely described

	This is generally used for kernels requiring a delay line like in a FIR filter.
*/
Object_T *KerArgPad(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int BottomBuffer,	/**< Amount of buffer to be added before this kernel argument, unit is ItemSize */
	unsigned int TopBuffer,		/**< Amount of buffer to be added after this kernel argument, unit is ItemSize */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);

#ifdef LEGACY
/**
@brief Creates one user kernel argument.  Kernel width/height is strided. Kernel argument Space is explicitely described

Creates one user kernel argument.  Kernel width/height is strided. Kernel argument Space is explicitely described
*/
Object_T *KerArg2D(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Stride,		/**< Kernel argument W is strided with stride factor = Stride in bytes */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);

/**
@brief Creates one user kernel argument with fixed padding left/right and top/bottom for this argument. Kernel width/height is strided. Kernel argument Space is explicitely described

Creates one user kernel argument with fixed padding left/right and top/bottom for this argument. Kernel argument Space is explicitely described

*/
Object_T *KerArg2DP(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int Stride,		/**< Kernel argument W is strided with stride factor = Stride in bytes */
	unsigned int PadL,		/**< Amount of padding to be added on the left of a line, unit is Item */
	unsigned int PadR,		/**< Amount of padding to be added on the right of a line, unit is Item */
	unsigned int PadT,		/**< Amount of padding to be added on the top of a column, unit is Item */
	unsigned int PadB,		/**< Amount of padding to be added on the bottom of a column, unit is Item */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);

Object_T *_KerArg2D(
	char *KerArgName,
	KernelArgDimDescrT *KerArgSpace,
	Object_Type_T ObjType,
	unsigned int W,
	unsigned int H,
	unsigned int Stride,
	v4s PadTile,
	v4s PadExec,
	unsigned int ItemSize,
	int TileOverlap,
	KernelArgConstraints_T Constraint,
	unsigned int PreferedTileSize,
	char *CArgName
	);
#endif
/**
@brief Creates one user kernel argument with dimension [WxH] but used partially as [UsedWxUsedH]. Kernel argument Space is explicitely described

Creates one user kernel argument with dimension [WxH] but used partially as [UsedWxUsedH]. Kernel argument Space is explicitely described

	This is generally used for kernels having vertically tiled arguments that are consumming only a subset of the whole
data plane. In the horizontal case it is not an issue since H can be safely reduced because arrays have line major layout but
for vertical we need to be aware that the used width is in fact in a data planne that is wider.
*/
Object_T *KerArgPart(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int UsedW,		/**< Kernel argument Data plane effective width, has to be < W */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int UsedH,		/**< Kernel argument Data plane effective height, has to be < H */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);


/** @} */ // End of UserKArg group

/* User kernel */
/**
@brief Creates a user kernel

Creates a user kernel
*/
Kernel_T *UserKernel(
	char *TemplateName,		/**< C name to be used for this user kernel */
	KernelIterationSpaceT *KerIter,	/**< See KernelIterationOrder() */
	Tile_Orientation_T Orientation,	/**< Kernel tiling orientation */
	CKernel_Arg_T **CArg,		/**< See Carg() and TCArg() */
	CKernelCall_T **CCalls,		/**< See Calls() and Call() */
	Object_T **KerArg		/**< See KerArgs() and KerArg(), KerArgPad() */
	);

/**
@brief Add informations to selected user kernel

Add informations to selected user kernel
*/
void AddKernelInfos(
	char *Name,			/**< A defined User Kernel Name */
	AT_KernelInfo_T Info,		/**< Which type of information */
	int NumVal,			/**< Value passed as an integer  */
	void *Val			/**< Value passed as a void * */
	);

/**
@brief Alter the behaviour of UserKernel processing

Alter the behaviour of UserKernel processing
Possible Ctrl values:
        AT_KERNEL_BUFFER_PROMOTE,             When all user kernel arguments can fit into given L1 memory promote them to buffer, default is 1
        AT_KERNEL_PARTIAL_BUFFER_PROMOTE,     When all tile of a user kernel argument across Input Features can fit into given L1 memory promote them to partial buffer, default is 1
        AT_KERNEL_NOSOLUTION_ERROR,           Report an error when no tiling solution is found, default is 1
For Val use Use APT_OPT_ON, AT_OPT_OFF, AT_OPT_VAL(Val)
*/
void AT_SetKernelCtrl(
	AT_GraphCtrl_T Ctrl,	/**< Which option */
	void *Val		/**< Value for this option. Use APT_OPT_ON, AT_OPT_OFF, AT_OPT_VAL(Val) */
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

Object_T *KerGroupArg(
	char *KerArgName,		/**< User kernel group argument name */
	Object_Type_T ObjType,		/**< Argument type, O_IN and/or O_OUT can be used only */
	int ArgSize,			/**< Number of elements of this argument */
	int ItemSize,			/**< Size in byte of the elementary data type of this argument */
	char *CArgName			/**< Corresponding name in the User Kernel Group definition */
	);

/**
@brief Binds argument to a user kernel group argument and combine it with Value using Oper

Binds argument to a user kernel group argument and combine it with Value using Oper

See K_Arg() for ArgName and ArgSelect. Supported operation are defined in ArgBindingOper. Value is a signed immediate constant.
*/
ArgBindingDescr_T *KG_ArgOper(
	char *ArgName,			/**< A tiled user kernel argument name */
	char Oper,			/**< Operation, see ArgBindingOper. Valid: + - * / %  */
	int Value			/**< A signed immediate value */
	);

/* A user kernel group */
/**
@brief Declare a user kernel group

Declare a user kernel group
*/
void UserKernelGroup(
	char *GroupName,		/**< Group's name as in OpenKernelGroup() */
       	CKernel_Arg_T **CArg,		/**< See Carg() and TCArg() */
       	CKernelCall_T **CCalls		/**< See Calls() and UserKernelCall() */
	);

KernelGroup_T *UserKernelGroupK(
	char *GroupName,		/**< Group's name as in OpenKernelGroup() */
	unsigned int IterCount,		/**< Number of time this group should be iterated, usually 1 but used for grouped convolution */
       	CKernel_Arg_T **CArg,		/**< See Carg() and TCArg() */
       	CKernelCall_T **CCalls,		/**< See Calls() and UserKernelCall() */
        Object_T **KerArg		/**< Kernel Group arguments. Restricted to KerGroupArg() as argument of KerArgs() */
	);

/** @} */ // End of UserKernelGroup group

/* CNN Graph */
/** @defgroup CNNG CNN_Graph
CNN Graph related functions
@{ */


/**
@brief Set graph processing options

Set graph processing options
Possible Ctrl are:
        AT_GRAPH_MONITOR_CYCLES,                Enable automatic cycle capture for each node of the graph, default is 0
        AT_GRAPH_MONITOR_CVAR_NAME,             When monitor cycles is on name of the C var array to receive results, default is AT_GraphPerf
        AT_GRAPH_PRODUCE_NODE_NAMES,            Enable production of an array containing the name of each graph node, default is 0 
        AT_GRAPH_PRODUCE_NODE_CVAR_NAME,        When producing node names is on name of the C array receiving the names as strings, default is AT_GraphNodeNames
        AT_GRAPH_PRODUCE_OPERINFOS,             Enable production of number of macs for each layer, default is 0
        AT_GRAPH_PRODUCE_OPERINFOS_CVAR_NAME,   When Number of oper Infos is on name of the C array receiving mac infos for each node, default is AT_GraphOperInfosNames
        AT_GRAPH_REORDER_CONSTANT_IN,           Enable reodering of constant inputs in order to transform 2D accesses into 1D accesses, default is 1
        AT_GRAPH_TRACE_EXEC,                    Enable trace of activity, default is 1
        AT_GRAPH_NOINLINE_NODE,                 If 1 all user kernel function is marked as noinline, default is 0
        AT_GRAPH_PREF_L3_EXEC,                  In case a symbol must be allocated in L3 for execution this is the prefered memory, default is AT_MEM_L3_HRAM
	AT_GRAPH_CONST_EXEC_FROM_FLASH,         If 1, for constant symbol executes from home location, default is 0
        AT_GRAPH_PREF_L3_HOME,                  For constant symbols which L3 flash prefered memory, default is AT_MEM_L3_HFLASH

For Val use Use APT_OPT_ON, AT_OPT_OFF, AT_OPT_VAL(Val)
*/
void AT_SetGraphCtrl(
	AT_GraphCtrl_T Ctrl,	/**< Which option */
	void *Val		/**< Value for this option. Use APT_OPT_ON, AT_OPT_OFF, AT_OPT_VAL(Val) */
	);

/**
@brief Returns the name of the defined graph

Returns the name of the defined graph
*/
char *CNNGraphName();

/**
@brief Returns the defined graph

Returns the defined graph
*/
CNNGraph_T *CNNGraph();


/**
@brief Declare a new CNN Graph and open it

Declare a new CNN Graph and open it
*/
CNNGraph_T *CreateGraph(
	char *GraphName,		/**< CNN Graph name */
	CKernel_Arg_T **CArgs,		/**< C arguments of this CNN graph */
	CKernel_Arg_T **Locals		/**< Local arguments of this CNN graph */
	);

/**
@brief Add a layer to the opened CNN graph

Add a layer to the opened CNN graph
*/
void AddNode(
	char *NodeName,			/**< A user kernel name or a user kernel group name */
	ArgBindingDescr_T **BindingList	/**< Bindings for this kernel call */
	);

/**
@brief Add a C call before or after a graph node

Add a C call before or after a graph node. Calls order follows insertion order
*/
void AddCallToNode(
	char *NodeName,			/**< Existing graph node name */
	CallLocationT Where,		/**< Before: CALL_PROLOG. After: CALL:EPILOG */
	char *FunName,			/**< Called function name */
	ArgBindingDescr_T **FunArgs	/**< Function Argument list, use GArg */
	);
/**
@brief Binds a given Graph node arg to a user kernel kernel argument 

Binds a given Graph node arg to a user kernel kernel argument 
*/
ArgBindingDescr_T *GNodeArg(
	GraghNodeArgT Type,		/**< Direction: GNA_IN, GNA_OUT or GNA_INOUT*/
	char *ArgName,			/**< Argument name, should be in internal or external graph variables and a user kernel kernel argument */
	char *AliasedArgName		/**< In case Direction is GNA_INOUT output ArgName is aliased to input AliasedArgName */
	);

/**
@brief Binds a given Graph node arg to a user kernel kernel C argument 

Binds a given Graph node arg to a user kernel kernel C argument 
*/
ArgBindingDescr_T *GNodeCArg(
	char *ArgName			/**< Argument name, should be an external graph variable and a user kernel C argument */
	);


/**
@brief Binds a given Graph node arg applying an immediate operation to it

Binds a given Graph node arg applying an immediate operation to it
*/
ArgBindingDescr_T *GNodeArgImmOper(
	GraghNodeArgT Type,		/**< Direction: GNA_IN, GNA_OUT or GNA_INOUT*/
	char *ArgName,			/**< Argument name, should be in internal or external graph variables */
	char *AliasedArgName,		/**< In case Direction is GNA_INOUT output ArgName is aliased to input AliasedArgName */
	char Oper,			/**< Offset operation to be applied */
	int Value			/**< Offset value */
	);

/**
@brief Binds a given Graph node arg, simplified form

Binds a given Graph node arg, simplified form
*/
ArgBindingDescr_T *GArg(
	char *ArgName			/**< Argument name, should be in internal or external graph variables */
	);

/**
@brief Close the currently open CNN Graph and process it

Close the currently open CNN Graph and process it
*/
void CloseGraph();

/**
@brief Generate code for the CNN Graph if any

Generate code for the CNN Graph if any
*/
void GenerateCodeForCNNGraph(
	CNNGraph_T *Graph,	/**< Pointer to the CNN graph */
	int Declare,		/**< if 1 declare graph related entry points, if 0 generate bodies */
	FILE *Fi		/**< File where to dump generated code */
	);


/** @} */ // End of CNN_Graph group

/* CNN Kernels */
/** @defgroup CNNG CNN_Kernels
CNN Kernels operation related functions
@{ */


/**
@brief Initializes a CNN Generator control descriptor, all fields are set to default

Initializes a CNN Generator control descriptor, all fields are set to default
*/
void CNN_InitGenCtrl(
	CNN_GenControl_T *Ctrl	/**< Address of a CNN generator control descriptor */
	);

/**
@brief Overides default behaviour of a CNN Generator. Set one CNN generator control descriptor field described by it's name to Val

Overides default behaviour of a CNN Generator. Set one CNN generator control descriptor field described by it's name to Val
Name is case insensitive, list of names
TileOrientation		Controls tiling orientation. -1: use default, TILE_HOR or TILE_VER
ParallelFeatures	Controls parallelization strategy. -1: use default, 0: Each output feature is evaluated in parallel, 1: several output features are evalauted in parallel 
ForcedDPConv		Controls double precision option for convolution: -1: use default, 0: Disable double precision, 1: Enable double precision
UseHwCE			Controls HWCE usage: -1: use default, 0: Disable HWCE, 1: Enable HWCE and use it if possible
PadType			Controls padding strategy: left/right/balanced_left/balanced_right
EnableIm2Col		Controls if Matrix Multiply based convolution should be used when possible
*/
void CNN_SetGenCtrl(
	CNN_GenControl_T *Ctrl,	/**< Address of a CNN generator control descriptor */
	char *Name,		/**< Field name, case insensitive */
	void *Val);		/**< Value, an integer or a string. Use APT_OPT_ON, AT_OPT_OFF, AT_OPT_VAL(Val)  */

/**
@brief Overides default behaviour of a CNN Generator. Set list of CNN generator control descriptor fields (Name, Val) stops when Name = 0

Overides default behaviour of a CNN Generator. Set list of CNN generator control descriptor fields (Name, Val) stops when Name = 0
*/
void CNN_SetGenCtrlList(
	CNN_GenControl_T *Ctrl,	/**< Address of a CNN generator control descriptor */
	...			/**< char *Name, void *Val pairs, stops when Name == 0 */
	);



/**
@brief Create a CNN type size vector(4)

Create a CNN type size vector(4).
Usually the 4 operands are In,Filter,Bias,Out. Byte size currently limited to 4
*/
int *CNN_Type(
	int I1,			/**< First operand type size in byte, 0 to ignore */
	int I2,			/**< Second operand type size in byte, 0 to ignore */
	int I3,			/**< Third operand type size in byte, 0 to ignore */
	int I4,			/**< Fourth operand type size in byte, 0 to ignore */
	int O);			/**< Output operand type size in byte, 0 to ignore */



/**
@brief Create a list of N CNN operation

Create a list of N CNN operation
*/
KernelOper_T *CNN_OperList(
	int N,			/**< Number of CNN operations */
	...			/**< List of CNN Operations, see KernelOper_T */
	);

/**
@brief Create a CNN matching condition for a Basic Kernel

Create a CNN matching condition for a Basic Kernel
*/
CNN_LayerOp_T *CNN_Match(
        KernelOper_T *KerOper1,	/**< List of N_Oper1 primary CNN operation matching */
        KernelOper_T *KerOper2,	/**< Number of N_Oper2 secondary CNN operation matching */
        int ParFeat,		/**< If basic kernel evaluates channels in parallel */
        int *OpType,		/**< Operand's type size: In1,In2,In3,Out usually (In,Filter,Bias,Out). 0 to ignore otherwise size in bytes */
        int Fx,			/**< Filter X dimension, 0 if don't care, -1 if match all input */
        int Fy,			/**< Filter Y dimension, 0 if don't care, -1 if match all input, -2 match all input by equal to Fx */
        int Dx,			/**< Filter X dilation, 0 if don't care, -1 if match all input */
        int Dy,			/**< Filter Y dilation, 0 if don't care, -1 if match all input, -2 match all input by equal to Dx */
        int Sx,			/**< Filter X stride, 0 if don't care, -1 if match all input */
        int Sy			/**< Filter Y stride, 0 if don't care, -1 if match all input, -2 match all input by equal to Sx */
	);


/**
@brief Look into KernelLibs operation descriptor, return best match if any

Look into KernelLibs operation descriptor, return best match if any. When a basic kernel can perform 2 sucessive operations
the 2nd operation can be specified.
*/
char *CNN_FindMatchingKernel(
        KernelOper_T KerOper1,	/**< Primary operation */
        KernelOper_T KerOper2,	/**< Optional operation to be performed on the output of the primary one */

        int ParallelOutFeat,	/**< Choose a kernel using channel centric parallelization, otherwise feature centric parallelization */

        int I1Size,		/**< Size in byte of the first input operand, 0 if not relevant. Order assumption: In,Filter,Bias,Out */
        int I2Size,		/**< Size in byte of the second input operand, 0 if not relevant */
        int I3Size,		/**< Size in byte of the third input operand, 0 if not relevant */
        int I4Size,		/**< Size in byte of the fourth input operand, 0 if not relevant */
        int OSize,		/**< Size in byte of the output input operand, 0 if not relevant */

        int FX,			/**< If this operation is a filter, Filter X dimension, 0: not relevant, -1: All value are matching, >0 a specific dimension */
        int FY,			/**< If this operation is a filter, Filter Y dimension, 0: not relevant, -1: All value are matching, -2 all values but equal to FX, >0 a specific dimension */
        int DX,			/**< If this operation is a filter, Filter dilation for X dimension, 0: not relevant, -1: All value are matching, >0 a specific dimension */
        int DY,			/**< If this operation is a filter, Filter dilation for Y dimension, 0: not relevant, -1: All value are matching, -2 all values but equal to FX, >0 a specific dimension */
        int SX,			/**< If this operation is a filter, Filter Stride for X dimension, 0: not relevant, -1: All value are matching, >0 a specific dimension */
        int SY,			/**< If this operation is a filter, Filter Stride for Y dimension, 0: not relevant, -1: All value are matching, -2 all values but equal to FX, >0 a specific dimension */

        int *NeedFx,		/**< FX should be explictly passed to the basic kernel */
        int *NeedFy,		/**< FY should be explictly passed to the basic kernel */
        int *NeedDx,		/**< DX should be explictly passed to the basic kernel */
        int *NeedDy,		/**< DY should be explictly passed to the basic kernel */
        int *NeedSx,		/**< SX should be explictly passed to the basic kernel */
        int *NeedSy,		/**< SY should be explictly passed to the basic kernel */
        int *ArgCount		/**< Nunber of arguments of the basic kernel implementing this CNN operation */
        );

/**
@brief Returns a C type for an argument given it's size in byte

Returns a C type for an argument given it's size in byte
*/
char *CNN_ArgDataType(
	int DataSize,		/**< Argument size in byte (1,2 or 4) */
	int Pointer,		/**< Is this argument a pointer */
	int Restrict		/**< In case this argument is a pointer can it be restricted? */
	);
/**
@brief For merged CNN layers retrieves composite Layer operation from individual operations.

For merged CNN layers retrieves composite Layer operation from individual operations.
*/

KernelOper_T CNN_CompositeKernel(
	KernelOper_T K1,	/**< First CNN operation */
	KernelOper_T K2,	/**< Second CNN operation or KOP_NONE */
	KernelOper_T K3		/**< Third CNN operation or KOP_NONE */
	);

/**
@brief Returns CNN Oper Name

Returns CNN Oper Name
*/
char *CNN_KernelOperImage(
	KernelOper_T Op		/**< A CNN operation */
	);

/** @} */ // End of CNN_Kernels


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
	const char *Message,
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
                    int FScW, int FScH, int ConvStrideW, int ConvStrideH, int DcW, int DcH, v4s PadInp,
                    int FSpW, int FSpH, int PoolStrideW, int PoolStrideH, int DpW, int DpH, v4s PadInc,
                    KernelOper_T KerOper,
                    int Norm,
                    int NormBias);
extern void AT_TestFinalize();

extern void DecodeCNNOper(
	KernelOper_T Oper,
	int *DoConv,
	int *IsDWconv,
	int *IsDPconv,
	int *DoPool,
	int *DoLinear,
	int *DoReLU,
	int *DoMatAdd,
	int *DoMatMul,
	int *DoSoftMax);


#endif
