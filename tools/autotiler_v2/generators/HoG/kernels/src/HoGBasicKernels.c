/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <math.h>
#include "HoGBasicKernels.h"



/*
	Compute HoG features: Normalized Blocks of Cells of gradient histogram.

	Key parameters for HOG are defined in HoGParameters.h:

	HOG_CELL_SIZE 				Size of the side of a square cell of pixels, total pixels in cells is HOG_CELL_SIZE^2 
	HOG_BLOCK_SIZE 				Size of the side of a square block of pixel cells, Total size in pixels is (HOG_BLOCK_SIZE^2*HOG_CELL_SIZE^2) 
	HOG_BLOCK_OVERLAP 			Overlapping factor between 2 adjacent blocks 
	HOG_ANGLE_STEP 				Number of degrees between 2 bins of the gradient histograms 
	HOG_NBINS = (180/HOG_ANGLE_STEP) 	Total number of bins for 0..180 degree, we use unsigned orientations

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

	Both KerProcessCellLine() and KerProcessBlockLine() are vectorized and parallel implementations-

	Sobel filter extends +/-1 around central point so we need to provision for it,
	A line length, without padding, has to divisible by Cell Size and the padded line size needs to be disible by 4,
	this last constraints comes from the fact that we access data as vectors of 4 pixels.
	A column height without padding has be disible by Cell Size

	On a typical image the cycle per pixel budget is approx 14 cycles running on 8 cores and using Atan2Order3().
	For a QVGA image (320x240) the number of cycles will be approx 1000000,
	in other words assuming 30 fps you will need to run the chip at at least 30MHz to sustain real time.

*/

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = rt_nb_pe();
	Log2Core = gap8_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

/* 
	Context tuned Atan2, X and Y are coming from 2d Sobel on 8 bits pixels => Max dynamic of both is [0..4*255] ok on 10 bits
	Precision is around 7 percent 
	Returns an angle in [0..180]
*/
static inline int Atan2Order1(int Y, int X)

{
#define Abs(x) 		(((x)<0)?-(x):(x))
	int R, Angle, Off;
	int AbsY = Abs(Y);	/* We assume we never have X=Y=0, if not true should use Max(1, Abs(Y)) to cover the atan2(case) */
	int C1 = To_Q(M_PI/4, 10);
	int C2 = To_Q((3*M_PI)/4, 10);
	
	if (X>=0) {
		R = ((X-AbsY)<<15)/(X+AbsY);
		Angle = C1<<10;
	} else {
		R = ((X+AbsY)<<15)/(AbsY-X);
		Angle = (C1*3)<<10;
	}
	/* R is in Q15.15, return to Q15.10 */
	R = R>>5;
	Angle = gap8_roundnorm(Angle - C1*R, 10);
	if (Y<0) Angle = -Angle;
	Angle = (Angle*To_Q(180.0/M_PI, 10))>>10;
	if (Angle<0) Off=180<<10;
	else Off=0;
	Angle = gap8_addroundnorm(Off, Angle, 10);
	return Angle;
#undef Abs
}


/* 
	Context tuned Atan2, X and Y are coming from 2d Sobel on 8 bits pixels => Max dynamic of both is [0..4*255] ok on 10 bits
	Precision is around 1 percent 
	Returns an angle in [0..180]
*/
static inline int Atan2Order3(int Y, int X)

{
#define Abs(x) 		(((x)<0)?-(x):(x))
	int C, R, Angle, Off;
	int AbsY = Abs(Y);	/* We assume we never have X=Y=0, if not true should use Max(1, Abs(Y)) to cover the atan2(case) */

	if (X>=0) {
		R = ((X-AbsY)<<15)/(X+AbsY); C = 1;
	} else {
		R = ((X+AbsY)<<15)/(AbsY-X); C = 3;
	}
	/* R is in Q15.15, return to Q15.10 */
	R = R>>5;
        Angle = gap8_roundnorm(To_Q(0.1963, 10) * (gap8_roundnorm(R*R*R, 2*10)) - To_Q(0.9817, 10)*R + C*To_Q(M_PI/4, 2*10), 10);

	if (Y<0) Angle = -Angle;

        Angle = (Angle*To_Q(180.0/M_PI, 10))>>10;

	if (Angle<0) Off=180<<10; else Off=0;
	Angle = gap8_addroundnorm(Off, Angle, 10);
	return Angle;
#undef Abs
}

static void __attribute__ ((noinline)) ProcessOneCell(unsigned char *In, unsigned short *Out, unsigned int W)

{
#define Abs(x) ((x<0)?(-x):x)
	static int Debug = 0;
	int CellCol, CellLine;
	unsigned int B0, B1;
		/* Gy Sobel Kernel */
        v4s C0 = (v4s) {-1, -2, -1, 0};
        v4s C1 = (v4s) { 1,  2,  1, 0};
                /* Gx Sobel Kernel */
        v4s C2 = (v4s) {-1,  0,  1, 0};
        v4s C3 = (v4s) {-2,  0,  2, 0};
        v4s C4 = (v4s) {-1,  0,  1, 0};

    for(int i=0;i<HOG_NBINS;i++) Out[i]=0;

	for (CellCol=0; CellCol<HOG_CELL_SIZE; CellCol++) {
		/* Prime the Sobel filters, coeffs for x and y dir are  C0, ..., C4 as vectors of 3 values padded with one 0,
		   we need 3 adjacent lines of data, for a new filter evaluation we fill only the last vector with fresh data
		   and push the previously loaded ones from V2 to V1 and V1 to V0. The same input data (V0,V1,V2) is used for both
		   directions */
		
		v4u *VectIn = (v4u *) (In + CellCol);
		v4u V0;
		v4u V1 = *VectIn; 
		VectIn = (v4u *) (In + CellCol+ W);
		v4u V2 = *VectIn;

		for (CellLine=0; CellLine<HOG_CELL_SIZE; CellLine++) {
			int Vx, Vy;
			unsigned int M, AbsVx, AbsVy;
			V0 = V1; V1 = V2;
			VectIn = (v4u *) (In + CellCol + W*(2+CellLine));
			V2 = *VectIn; 
			
			/* y gradient, we don't need to evaluate the middle line since all coeffs are 0 */
			Vy = gap8_dotpus4    (V0, C0);
			Vy = gap8_sumdotpus4 (V2, C1, Vy);
			/* x gradient */
			Vx = gap8_dotpus4    (V0, C2);
			Vx = gap8_sumdotpus4 (V1, C3, Vx);
			Vx = gap8_sumdotpus4 (V2, C4, Vx);
			/* Compute L1 norm of the gradient */
			AbsVx = Abs(Vx); AbsVy = Abs(Vy);
			/* The dynamic of the Gradient Magnitude is reduced to both avoid costly Atan2 and
			   also to make sure that after accumulation in cells and in blocks we remain within 16 bits */
			M =  (AbsVx+AbsVy)>>HOG_GRADIENT_SHIFT;
			if (M > 0) {
				unsigned int Bin, Alpha;
				unsigned int O = Atan2Order3(Vy, Vx);
				
				// O = Atan2Order1(Vy, Vx);
				/* Divide orientation by angle between 2 bins, use multiplication by reciprocal of HOG_ANGLE_STEP */
				Bin = gap8_muluN(O, INV_HOG_ANGLE_STEP, HOG_DYNA);
				/* Atan2 can return 180 deg, make it equivalent to 0 if it happens */
				if (Bin==HOG_NBINS) Bin = 0;
				/* Figure out the ratio to split M between Bin and Bin+1, Alpha*M goes to bin, (1-Alpha)*M goes to Bin+1, linear interpolation */
				Alpha = ((1<<HOG_DYNA)-1) - (O-Bin*HOG_ANGLE_STEP)*INV_HOG_ANGLE_STEP;
				B0 = gap8_muluRN(M, Alpha, HOG_DYNA);
				B1 = gap8_muluRN(M, (((1<<HOG_DYNA)-1)-Alpha), HOG_DYNA);
				if (Bin<(HOG_NBINS-1)) {
					/* Assign M*Alpha, M*(1-Alpha) to Out[Bin, Bin+1], do it as a vector write */
					*((v2u *)(Out + Bin)) += gap8_pack2(B0, B1);
				} else {
					/* We are on the last bin so the next one is the first one */
					Out[Bin] += B0; Out[0] += B1;
				}
				if (Debug) {
					unsigned int O0=Out[Bin], O1=Out[(Bin==HOG_NBINS)?0:(Bin+1)];
					unsigned int S0=O0+B0, S1=O1+B1;
					if (S0>>16) printf("Overflow on S0:%X, Out[Bin]: %X, Add: %X\n", S0, O0, B0);
					if (S1>>16) printf("Overflow on S1:%X, Out[Bin+1]: %X, Add: %X\n", S1, O1, B1);
				}
			}
		}
	}
	
#undef Abs
}
/*
	Lines from the input image (In) comes into full width (W) strips,
	height of the strip is CellLineCount*HOG_CELLSIZE + 2, the extra 2 comes from the Sobel filter extension

	LineCellArray contains HOG_BLOCK_SIZE of evaluated lines of cells. Each time a new line is evaluated we pushed it
	into LineCellArray, if CellLineIndex is >= HOG_BLOCK_SIZE we shift all the line down by one position
*/
void KerProcessCellLine(KerProcessCellLine_ArgT *Arg)

{
	unsigned char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned short int ** __restrict__ LineCellArray = Arg->CellLines;
	unsigned int CellLineIndex = Arg->CellLineIndex;
	unsigned int CellLineCount = Arg->CellLineCount;

	unsigned int CellCount = (W-2)/HOG_CELL_SIZE;
	unsigned int CoreId = rt_core_id();
	unsigned int ChunkCell = ChunkSize(CellCount);
	unsigned int First = CoreId*ChunkCell, Last  = Min(CellCount, First+ChunkCell);

	unsigned short int *CellLine;
	unsigned int i, k;
	
	for (k=0; k<CellLineCount; k++) {
		if (CoreId==0) {
			if (CellLineIndex>=HOG_BLOCK_SIZE) {	
				CellLine = LineCellArray[0];
				for (i=1; i<HOG_BLOCK_SIZE; i++) LineCellArray[i-1] = LineCellArray[i];
					LineCellArray[HOG_BLOCK_SIZE-1] = CellLine;
			}
		} 
		rt_team_barrier();
        if (CellLineIndex>=HOG_BLOCK_SIZE) CellLine = LineCellArray[HOG_BLOCK_SIZE-1];
        else CellLine = LineCellArray[CellLineIndex];

		for (i=First; i<Last; i++) ProcessOneCell(In + k*W*HOG_CELL_SIZE + i*HOG_CELL_SIZE, CellLine + i*HOG_NBINS, W);
	
        rt_team_barrier();
        CellLineIndex++;
    }
    
}

static void __attribute__ ((noinline)) ProcessOneBlock(unsigned short **__restrict__ CellLines, unsigned int CellOffset, unsigned short *__restrict__ HOG_Features)

{
	unsigned int Col, Line;
	unsigned int Sum, InvSum;

	/*
	   Compute the normalization factor, first we get the sum of gradient contributions in the block.
	   We use a vector dot product by {1, 1} to speed up the sum
	*/
	Sum = 0;
	for (Line=0; Line<HOG_BLOCK_SIZE; Line++) {
		v2u *BlockLineVect = (v2u *) (CellLines[Line]+CellOffset);
		for (Col=0; Col<((HOG_BLOCK_SIZE*HOG_NBINS)>>1); Col++) {
			Sum = gap8_sumdotpu2(BlockLineVect[Col], ((v2u) {1,1}), Sum);
		}
	}
	/*
	    Normalization factor is 1/Sum represented as a UQ1.16,
	    Instead of dividing (cost) we multiply by the reciprocal
	    No solution here to vectorize
	*/
	if (Sum!=0) InvSum = (1<<16)/Sum; else InvSum=0;
	for (Line=0; Line<HOG_BLOCK_SIZE; Line++) {
		unsigned short *BlockLine = (CellLines[Line]+CellOffset);
		for (Col=0; Col<(HOG_BLOCK_SIZE*HOG_NBINS); Col++) {
			//HOG_Features[(HOG_BLOCK_SIZE*HOG_NBINS)*Line+Col] = gap8_roundnormu(BlockLine[Col]*InvSum, 16);
			HOG_Features[(HOG_BLOCK_SIZE*HOG_NBINS)*Line+Col] = BlockLine[Col]*InvSum;
		}
	}
}

void KerProcessBlockLine(KerProcessBlockLine_ArgT *Arg)

{
	unsigned short int ** __restrict__ CellLines = Arg->CellLines;
	unsigned int W = Arg->W;
	unsigned short int * __restrict__ HOGFeatures = Arg->HOGFeatures;

	unsigned int CellW = (W-2)/HOG_CELL_SIZE;					// Number of cells in a line
	unsigned int BlockW = (((CellW - HOG_BLOCK_SIZE)/HOG_BLOCK_CELL_STRIDE) + 1);	// Number of blocks in a line
	unsigned int BlockSize = HOG_NBINS*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE;		// Size of a single block


	unsigned int CoreId = rt_core_id();
	unsigned int ChunkBlock = ChunkSize(BlockW);
	unsigned First = CoreId*ChunkBlock;
	unsigned Last  = Min(BlockW, First+ChunkBlock);
	unsigned int Col;

	/* Compute a line of HoG block features */
	for (Col=First; Col<Last; Col++)
		ProcessOneBlock(CellLines, Col*HOG_BLOCK_CELL_STRIDE*HOG_NBINS, HOGFeatures+Col*BlockSize);
	rt_team_barrier();
}