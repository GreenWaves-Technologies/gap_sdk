#ifndef __HOG_GENERATOR_H__
#define __HOG_GENERATOR_H__

/** @addtogroup groupHOG
@{ */

/** @defgroup HOGGene HOGGenerator
@{ */

/**
@brief Load HOG basic kernels

Load HOG basic kernels
*/
void LoadHOGLibrary();

/**
@brief Add HOG Estim basic kernel dependencies

Add HOG Estim basic kernel dependencies
*/
void HOGEstimDependencies();

/**
@brief Setup AutoTiler configuration for HOG

Setup AutoTiler configuration
*/
void HOGEstimConfiguration(
	unsigned int L1Memory	/**< Maximum amount of memory in byte usable by the AutoTiler in shared L1 */
	);

/**
@brief Generate HOG for a given HOG set of parameters and for a given image size

Generate HOG for a given HOG set of parameters and for a given image size
*/
void GenerateHOG(
	char *Name,			/**< C Name of the function AutoTiler will generate */
	unsigned int W,			/**< Image width, width%4==0 should hold */
	unsigned int H, 		/**< Image Height */
	unsigned int CellSize, 		/**< Cell is [CellSize x CellSize] pixels */
	unsigned int BlockSize, 	/**< Block is [BlockSize x BlockSize] Cells */
	unsigned int BlockOverlap, 	/**< Number Cells in common between 2 adjacent blocks */
	unsigned int BinsPerCell	/**< Number of bins for a cell */
	);

void GenerateWeakHOGEstimator(
                char *Name,
                unsigned int W,                 /* Image width */
                unsigned int H,                 /* Image Height */
                unsigned int CellSize,          /* Cell is [CellSize x CellSize] pixels */
                unsigned int BlockSize,         /* Block is [BlockSize x BlockSize] Cells */
                unsigned int BlockOverlap,      /* Number Cells in common between 2 adjacent blocks */
                unsigned int BinsPerCell,       /* Number of bins for a cell */
                unsigned int WEstimPix,         /* Width of the estimation window, in pixels */
                unsigned int HEstimPix,         /* Height of the estimation window, in pixels */
                unsigned int EstimSize,         /* Size of estimation result per estimation window, in bytes */
                unsigned int NWeakEstim         /* Number of weak estimators in the model */
        );

void GenerateWeakHOGEstimatorBis(
                char *Name,
                unsigned int W,                 /* Image width */
                unsigned int H,                 /* Image Height */
                unsigned int CellSize,          /* Cell is [CellSize x CellSize] pixels */
                unsigned int BlockSize,         /* Block is [BlockSize x BlockSize] Cells */
                unsigned int BlockOverlap,      /* Number Cells in common between 2 adjacent blocks */
                unsigned int BinsPerCell,       /* Number of bins for a cell */
                unsigned int WEstimPix,         /* Width of the estimation window, in pixels */
                unsigned int HEstimPix,         /* Height of the estimation window, in pixels */
                unsigned int EstimSize,         /* Size of estimation result per estimation window, in bytes */
                unsigned int NWeakEstim         /* Number of weak estimators in the model */
        );

void GenerateResize(
	char *Name,			/**< C Name of the function AutoTiler will generate */
	unsigned int Win,		/**< Source image width */
	unsigned int Hin,		/**< Source image height */
	unsigned int Wout,		/**< Destination image witdh */
	unsigned int Hout		/**< Destination image height */
	);

/** @} */ // End of HOGGene

/** @} */
#endif
