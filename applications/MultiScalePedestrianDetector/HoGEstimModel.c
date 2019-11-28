/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include <stdint.h>
#include <stdio.h>

/** @addtogroup groupHOG
@{ */

/** @defgroup HOGExa HOGExample

This section shows how to use the HOG Generator on 644x482 input image

@code
// AutoTiler Libraries
#include "AutoTilerLib.h"
// AutoTiler Standard types plus a set of pre defined types for basic kernels
#include "StdTypes.h"
// HOG parameters definitions
#include "HoGParameters.h"
// HOG generator
#include "HoGGenerator.h"

int main(int argc, char **argv)

{
	// This will parse AutoTiler options and perform various initializations
        if (TilerParseOptions(argc, argv)) {
                printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
        }
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 51200 bytes
	HOGConfiguration(51200);
	// Load the HOG basic kernels template library
        LoadHOGLibrary();
	// Call HOG generator, here image is [644x482], the HOG parameters come from HOGParameters.h
	unsigned int W = 644, H = 482;
	GenerateHOG("MyHOG", W, H, HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	// Now that we are done with model parsing we generate the code
        GenerateTilingCode();
        return 0;
}
@endcode

@{ */

// AutoTiler Libraries
#include "AutoTilerLib.h"
// HOG parameters definitions
#include "HoGParameters.h"
// HOG Estim parameters definitions
#include "HoGEstimParameters.h"
// HOG generator
#include "HoGEstimGenerator.h"
// Image resize generator
#include "ResizeGenerator.h"

int main(int argc, char **argv)

{
	// This will parse AutoTiler options and perform various initializations
        if (TilerParseOptions(argc, argv)) {
                printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
        }
	// Setup AutTiler configuration. Used basic kernel libraries, C names to be used for code generation,
	// compilation options, and amount of shared L1 memory that the AutoTiler can use, here 17000 bytes
	// HOGEstimConfiguration(12300);
	HOGEstimConfiguration(17000);
	// Load the HOG basic kernels template library
    LoadHOGLibrary();
	// Load Resize basic kernels and dependencies */
	LoadResizeLibrary();
    SetUsedFilesNames(0, 1, "ResizeBasicKernels.h");
	// Call HOG generator, here image is [320x240], the HOG parameters come from HOGParameters.h and HoGEstimParameters.h

	// unsigned int W = 640, H = 480;
	unsigned int W = 322, H = 242;
	unsigned int We = HOG_ESTIM_WIDTH;
	unsigned int He = HOG_ESTIM_HEIGHT;
	int l_v=0;//pyramid level value

	//preprocessing to enter the right image
	// First level of the pyramid, plain resolution, Generated HOG Features and HOG Estimator
	GenerateHOG             ("MyHOG",         W        , H        , HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim",W        , H        , HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
	// Second level of the pyramid, Plain resolution reduced by 1/8, Generates resize, HOG Features and HOG Estimator
	l_v=-1;
	GenerateResize	        ("MyResize_N1",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_N1",         (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_N1",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);

	// Third level of the pyramid, Plain resolution reduced by 2/8, Generates resize, HOG Features and HOG Estimator
	l_v=-2;
	GenerateResize	        ("MyResize_N2",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_N2",            (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)),    (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_N2",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);

	// Fourth level of the pyramid, Plain resolution reduced by 3/8, Generates resize, HOG Features and HOG Estimator
	l_v=-3;

	GenerateResize	        ("MyResize_N3",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_N3",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_N3", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);


/*
	// Fourth level of the pyramid, Plain resolution reduced by 3/8, Generates resize, HOG Features and HOG Estimator
	l_v=-4;

	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/
/*

	// Fourth level of the pyramid, Plain resolution reduced by 3/8, Generates resize, HOG Features and HOG Estimator
	l_v=-5;

	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/
/*
	// Fourth level of the pyramid, Plain resolution reduced by 3/8, Generates resize, HOG Features and HOG Estimator
	l_v=-6;

	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/
	// Fourth level of the pyramid, Plain resolution reduced by 3/8, Generates resize, HOG Features and HOG Estimator
/*	l_v=-7;

	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/
	// Level
/*	l_v=-8;

	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",             (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim", (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/

	// Fifth level of the pyramid, Plain resolution increaed by 1/8, Generates resize, HOG Features and HOG Estimator
	l_v=1;
	GenerateResize	        ("MyResize_1",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_1",            (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_1",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);

	// Sixth level of the pyramid, Plain resolution increaed by 2/8, Generates resize, HOG Features and HOG Estimator
	l_v=2;
	GenerateResize	        ("MyResize_2",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_2",            (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_2",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);

	l_v=3;
	GenerateResize	        ("MyResize_3",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG_3",            (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim_3",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);

	/*l_v=4;
	GenerateResize	        ("MyResize",      W        , H        , (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)));
	GenerateHOG             ("MyHOG",            (W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS);
	GenerateWeakHOGEstimatorBis("MyWeakHOGEstim",(W+l_v*(W/HOG_ESTIM_SCALE_FACTOR)), (H+l_v*(H/HOG_ESTIM_SCALE_FACTOR)), HOG_CELL_SIZE, HOG_BLOCK_SIZE, HOG_BLOCK_OVERLAP, HOG_NBINS, We, He, 1, HOG_WEAK_ESTIM);
*/

	// Now that we are done with model parsing we generate the code
        GenerateTilingCode();
        return 0;
}
/** @} */ // End of HOGExample
/** @} */

