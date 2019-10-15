/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __HOGBASICKERNELS_H__
#define __HOGBASICKERNELS_H__

#include "Gap8.h"
#include "HoGParameters.h"

/** @addtogroup groupHOG
@{ */

/// @cond PrivateStuff
/* Horizontal amount of padding for the input image, 2 are for sobel filter and 2 to make sure line is a multiple of 4 */
#define PAD_W   2
/* Vectical amount of padding, sobel filter extension */
#define PAD_H   2

/* Shift to be applied to Magnitude of Gradient, context dependant, 2 goals: limit calls to atan2 and make sure we stay on 16bits when accumulating  */
#define HOG_GRADIENT_SHIFT      4
/* Block size in pixels */
#define HOG_BLOCK_PIX_SIZE      (HOG_BLOCK_SIZE*HOG_CELL_SIZE)
/* Stride in Cells between 2 adjacent blocks */
#define HOG_BLOCK_CELL_STRIDE   (HOG_BLOCK_SIZE-HOG_BLOCK_OVERLAP)
/* Stride in pixels between 2 adjacent blocks */
#define HOG_BLOCK_PIX_STRIDE    (HOG_CELL_SIZE*HOG_BLOCK_CELL_STRIDE)
/* Number of bits to be used for the fractional part of expressions evaluated when computing HOG cells */
#define HOG_DYNA                15

/* 1/HOG_ANGLE_STEPS in fractionnal */
#define INV_HOG_ANGLE_STEP      (((1<<HOG_DYNA)-1)/HOG_ANGLE_STEP)


#define From_Q(Val, Dyna)       ((float) (((float) (Val))/((float) (1<<Dyna))))
#define To_Q(Val, Dyna)         ((int) ((float) Val * (float) (1<<Dyna)))

#ifndef Max
#define Max(a, b)               (((a)>(b))?(a):(b))
#endif
#ifndef Min
#define Min(a, b)               (((a)<(b))?(a):(b))
#endif

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))
/// @endcond


/** @defgroup HOGBasicK HOGBasicKernels

        Compute HoG features: Normalized Blocks of Cells of gradient histogram.



        Key parameters for HOG are defined in HoGParameters.h:

        HOG_CELL_SIZE                           Size of the side of a square cell of pixels, total pixels in cells is HOG_CELL_SIZE^2
        HOG_BLOCK_SIZE                          Size of the side of a square block of pixel cells, Total size in pixels is (HOG_BLOCK_SIZE^2*HOG_CELL_SIZE^2)
        HOG_BLOCK_OVERLAP                       Overlapping factor between 2 adjacent blocks
        HOG_ANGLE_STEP                          Number of degrees between 2 bins of the gradient histograms
        HOG_NBINS = (180/HOG_ANGLE_STEP)        Total number of bins for 0..180 degree, we use unsigned orientations


        This code operates on horizontal stripes. Each iteration produces a line of cells.

        A cell contains the histogram of gradient over a patch of HOG_CELL_SIZE x HOG_CELL_SIZE pixels from input image.

        Gradient is computed using a 2D Sobel filter. To obtain the gradient orientation from it's 2 vertical and horizontal
        projections 2 variants of atan2() can be used. The first one, Atan2Order3(), is a 3rd order approximation and computes the angle with
        max 1 degree of error. The second, Atan2Order1(), is a 1st order approximation and computes the angle with less than 7 degree of error.
        Histogram is discretized on HOG_NBINS angular bins over [0 Deg .. 180 Deg], each B bin contains the
        accumulated Magnitude of gradient having direction Angle(B), linear interpolation is performed between
        the 2 bins surrounding the actual gradient. For magnitude we use the L1 norm (sum of abs())

        A block of a cell is the concatenation of HOG_BLOCK_SIZE x HOG_BLOCK_SIZE Cells, normalization is perfomed
        on this group of cells. This block of cells containing HOG_BLOCK_SIZE x HOG_BLOCK_SIZE x NBINS is also called a HOG Feature

        The first phase primes the pipeline:

                HOG_BLOCK_SIZE lines of cells are produced, each line goes into a buffer of cell lines.
                Lines of cells are produced one by one by KerProcessCellLine() with Arg->CellLineCount = 1

                Once the HOG_BLOCK_SIZE lines of cells have been produced we can produce one line of HOG features.
                One line of block is produced by KerProcessBlockLine()

        The second phase is the pipeline body, at each iteration:

                HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP lines of cells are produced by KerProcessCellLine() with Arg->CellLineCount = HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP
                Each new line of cells is inserted at the top of the cell lines buffer after pushing down all the existing lines by one position.
                One line of block is produced by KerProcessBlockLine()

        Both KerProcessCellLine() and KerProcessBlockLine() are vectorized and parallel implementations

        Sobel filter extends +/-1 around central point so we need to provision for it.

        A line length, without padding, has to divisible by Cell Size and the padded line size needs to be disible by 4,
        this last constraints comes from the fact that we access data as vectors of 4 pixels.
        A column height without padding has be disible by Cell Size

        On a typical image the cycle per pixel budget is approx 14 cycles running on 8 cores and using Atan2Order3().

        For a QVGA image (320x240) the number of cycles will be approx 1000000,

        in other words assuming 30 fps you will need to run the chip at at least 30MHz to sustain real time.

@{ */

/**
@brief Arguments template for KerProcessCellLine()

Arguments template for KerProcessCellLine()
*/
typedef struct {
        unsigned char * __restrict__ In; /**< Pointer to input image stripe, one pixel is one byte */
        unsigned int W; /**< Width of the stripe, in pixels */
        unsigned short int ** __restrict__ CellLines; /**< Pointer of cell line buffer, HOG_BLOCK_SIZE entries */
        unsigned int CellLineIndex; /**< Which cell line in the image is processed */
        unsigned int CellLineCount; /**< Number of cell lines in the stripe, stripe height=HOG_CELL_SIZE * CellLineCount */
} KerProcessCellLine_ArgT;
/**
@brief Process one or several lines of cells

Process one or several lines of cells


        This functions receives an horizontal stripe from the input image. It's width is W and is height is (HOG_CELL_SIZE * CellLineCount) + 2.
        The +2 comes from the fact that a 2D Sobel filter is used so the stripes has to extend by 1 both on top and on bottom.
        Each processed line of cells is stored in CellLines (a buffer of HOG_BLOCK_SIZE lines of cells). If CellLineIndex < HOG_BLOCK_SIZE
        the new line of cells is inserted at entry CellLineIndex. If CellLineIndex >= HOG_BLOCK_SIZE the cell line buffer is first pushed
        to the bottom by one position and the new line of cells is inserted at entry HOG_BLOCK_SIZE-1
*/
void KerProcessCellLine(KerProcessCellLine_ArgT *Arg);

/**
@brief Arguments template for KerProcessBlockLine()

Arguments template for KerProcessBlockLine()
*/
typedef struct {
        unsigned short int ** __restrict__ CellLines; /**< Pointer to a buffer of cell lines having HOG_BLOCK_SIZE entries */
        unsigned int W; /**< Width in pixel of the input image */
        unsigned short int * __restrict__ HOGFeatures; /**< Pointer to a buffer of HOG Features */
} KerProcessBlockLine_ArgT;
/**
@brief Process a single full line of HOG Features

Process a single full line of HOG Features

        This functions receives a buffer, CellLines, of HOG_BLOCK_SIZE lines of cells and produces a full line of HOG features
*/
void KerProcessBlockLine(KerProcessBlockLine_ArgT *Arg);

/** @} */ // End of HOGBasicKernels
/** @} */
#endif
