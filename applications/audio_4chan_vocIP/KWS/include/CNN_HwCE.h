/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __CNN_HWCE_H__
#define __CNN_HWCE_H__

#include "Gap8.h"

#define HWCE_CONV3x3            1
#define HWCE_CONV5x5            0
#define HWCE_CONV4x7            2

#define HWCE_ONE_OUT		0x7
#define HWCE_TWO_OUT		0x3
#define HWCE_THREE_OUT		0x1
#define HWCE_FOUR_OUT		0x0

/* HWCE enabled convolutions

Nip: 		Number of input planes
Nop: 		Number of output planes

Filter: 	(2K+1) x (2K + 1)

Input plane: 	W x H
Output plane:	(W-2K) x (H-2K)

1) 3x3, Single output per cycle
   HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output
        is produced at a time
   Filter:
        (3*3 + Pad) * Nip * Nop coefficients
        Layout is:
                <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                <Coeff Group for OutPlanei> =
                        <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                <Coeffs for InPlanej> = Cij0 Cij1 ... Cij8 0

   Bias[Nop]: Bias to be used as an init value for each output of the outplane set
   Norm: fixed point precision

   Convolution output is W-2 x H-2 (no padding).
   Linear rectification followed by max pooling on a 2x2 grid is applied before sending results bask to memory
   Warning:
        W has to be a multiple of 2
        Each filter group (3x3) should be padded with one zero, this is a HWCE alignment constraint

2) 3x3, 2 outputs per cycle
   HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. Two convolution outputs
        Are produced at a time
   Filter:
        (3*3) * Nip * Nop coefficients
        Layout is:
                <Coeff Group for OutPlane01><Coeff Group for OutPlane23> ... <Coeff Group for OutPlaneNop-2Nop-1>
                <Coeff Group for OutPlaneii+1> =
                        <Coeffs for InPlane01><Coeffs for InPlane23> ... <Coeffs for InPlaneNip-2Nip-1>
                <Coeffs for InPlanejj+1> = Cij0 Ci+1j0 Cij1 Ci+1j1 ... Cij8 Ci+1j8

   Bias[Nop]: Bias to be used as an init value for each output of the outplane set
   Norm: fixed point precision

   Convolution output is W-2 x H-2 (no padding).
   Linear rectification followed by max pooling on a 2x2 grid is applied before sending results bask to memory
   Warning:
        W has to be a multiple of 2
        In this case no padding is needed for coefficients

3) 3x3, 3 outputs  per cycle
   HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. Three convolution outputs
        Are produced at a time
   Filter:
        (3*3) * Nip * Nop coefficients
        Layout is:
                <Coeff Group for OutPlane012><Coeff Group for OutPlane345> ... <Coeff Group for OutPlaneNop-3Nop-2Nop-1>
                <Coeff Group for OutPlaneii+1i+2> =
                        <Coeffs for InPlane012><Coeffs for InPlane345> ... <Coeffs for InPlaneNip-3Nip-2Nip-1>
                <Coeffs for InPlanejj+1j+2> = Cij0 Ci+1j0 Ci+2j0   Cij1 Ci+1j1 Ci+2j1  ...   Cij8 Ci+1j8 Ci+2j8

   Bias[Nop]: Bias to be used as an init value for each output of the outplane set
   Norm: fixed point precision

   Convolution output is W-2 x H-2 (no padding).
   Linear rectification followed by max pooling on a 2x2 grid is applied before sending results bask to memory
   Warning:
        W has to be a multiple of 2
        In this case padding is needed for coefficients, one zero added after 3x8=27 coefficients

4) 5x5, 1 output per cycle
   HWCE 5x5 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output
        is produced at a time
   Filter:
        (5*5 + Pad) * Nip * Nop coefficients
        Layout is:
                <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                <Coeff Group for OutPlanei> =
                        <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                <Coeffs for InPlanej> = Cij0 Cij1 ... Cij24 0

   Bias[Nop]: Bias to be used as an init value for each output of the outplane set
   Norm: fixed point precision

   Convolution output is W-4 x H-4 (no padding).
   Linear rectification followed by max pooling on a 2x2 grid is applied before sending results bask to memory
   Warning:
        W has to be a multiple of 2
        Each filter group (5x5) should be padded with one zero, this is a HWCE alignment constraint

5) 7x7, 1 output per cycle
   HWCE 7x7 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output
        is produced at a time
   Filter:
        (7*7 + Pad) * Nip * Nop coefficients
        Layout is:
                <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                <Coeff Group for OutPlanei> =
                        <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                <Coeffs for InPlanej> = Cij0 Cij1 ... Cij48 0

   Bias[Nop]: Bias to be used as an init value for each output of the outplane set
   Norm: fixed point precision

   Convolution output is W-6 x H-6 (no padding).
   Linear rectification followed by max pooling on a 2x2 grid is applied before sending results bask to memory
   Warning:
        W has to be a multiple of 2
        Each filter group (5x5) should be padded with one zero, this is a HWCE alignment constraint

*/

/** @addtogroup groupCNN
@{ */

/** @defgroup CNNHWCEBasicK CNNHWCEBasicKernels
@{ */


/** @brief Enable the HWCE

Enable the HWCE

Clock is released
*/
static inline void HWCE_Enable()

{
	plp_hwce_enable();
}

/** @brief Disable the HWCE

Disable the HWCE

Clock is paused (HW is clock gated)
*/
static inline void HWCE_Disable()

{
	plp_hwce_disable();
}

/** @brief Software reset the HWCE

Software reset the HWCE
*/
static inline void HwCE_SoftReset()

{
	__builtin_pulp_OffsetedWrite(1, (int *) HWCE_ADDR_BASE, HWCE_SOFT_CLEAR);
}

/** @brief Configure the HWCE

Configure the HWCE
*/
void HWCE_GenericInit(
	unsigned int ConvType,	/**< 0: 5x5 conv, 1: 3x3 conv, 2: 7x4 conv */
	unsigned int WStride,	/**< Width in pixels of the convolution input's data */
	unsigned int Norm	/**< Fixed point format, max is Q15. */
	);

/** @brief Set bias value

Set bias value
*/
void HwCE_SetInputBias(
	int Bias	/**< Bias value, fixed point, max is Q15. */
	);

/** @brief Control convolution Bias usage

Control convolution Bias usage

        When Disable = 1 a convolution result will not be accumulated with previous result but instead will be accumulated with
        a bias value that has been set before by HwCE_SetInputBias()
        When Disable = 0 a convolution result will be accumulated with previous result
*/
void HwCE_SetYinMode(
	unsigned int Disable	/**< 1: use bias, 0: use previous convolution result */
	);


/** @brief HWCE based 3x3 convolution, short inputs, outputs, coefficients and bias. 1, 2 or 3 outputs per cycle.


HWCE based 3x3 convolution, short inputs, outputs, coefficients and bias. 1, 2 or 3 outputs per cycle.

        1) 3x3, Single output per cycle
           HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output
                is produced at a time
           Filter:
                (3*3 + Pad) * Nip * Nop coefficients
                Layout is:
                        <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                        <Coeff Group for OutPlanei> =
                                <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                        <Coeffs for InPlanej> = Cij0 Cij1 ... Cij8 0

           Bias[Nop]: Bias to be used as an init value for each output of the outplane set
           Norm: fixed point precision

           Convolution output is W-2 x H-2 (no padding).
           Warning:
                W has to be a multiple of 2
                Each filter group (3x3) should be padded with one zero, this is a HWCE alignment constraint

        2) 3x3, 2 outputs per cycle
           HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. Two convolution outputs
                Are produced at a time
           Filter:
                (3*3) * Nip * Nop coefficients
                Layout is:
                        <Coeff Group for OutPlane01><Coeff Group for OutPlane23> ... <Coeff Group for OutPlaneNop-2Nop-1>
                        <Coeff Group for OutPlaneii+1> =
                                <Coeffs for InPlane01><Coeffs for InPlane23> ... <Coeffs for InPlaneNip-2Nip-1>
                        <Coeffs for InPlanejj+1> = Cij0 Ci+1j0 Cij1 Ci+1j1 ... Cij8 Ci+1j8

           Bias[Nop]: Bias to be used as an init value for each output of the outplane set
           Norm: fixed point precision

           Convolution output is W-2 x H-2 (no padding).
           Warning:
                W has to be a multiple of 2
                In this case no padding is needed for coefficients

        3) 3x3, 3 outputs  per cycle
           HWCE 3x3 convolution on Nip input planes, each of them is W x H, and Nop output planes. Three convolution outputs
                Are produced at a time
           Filter:
                (3*3) * Nip * Nop coefficients
                Layout is:
                        <Coeff Group for OutPlane012><Coeff Group for OutPlane345> ... <Coeff Group for OutPlaneNop-3Nop-2Nop-1>
                        <Coeff Group for OutPlaneii+1i+2> =
                                <Coeffs for InPlane012><Coeffs for InPlane345> ... <Coeffs for InPlaneNip-3Nip-2Nip-1>
                        <Coeffs for InPlanejj+1j+2> = Cij0 Ci+1j0 Ci+2j0   Cij1 Ci+1j1 Ci+2j1  ...   Cij8 Ci+1j8 Ci+2j8

           Bias[Nop]: Bias to be used as an init value for each output of the outplane set
           Norm: fixed point precision

           Convolution output is W-2 x H-2 (no padding).
           Warning:
                W has to be a multiple of 2
                In this case padding is needed for coefficients, one zero added after 3x8=27 coefficients
*/
void HWCE_ProcessOneTile3x3_MultiOut(
	short int *In,		/**< Pointer to input tile of dimension [W x H] */
	short int *Out0,	/**< Pointer to 1st output tile of dimension [W-2 x H-2] */
	short int *Out1,	/**< Null or pointer to 2nd output tile of dimension [W-2 x H-2] */
	short int *Out2,	/**< Null or pointer to 3rd output tile of dimension [W-2 x H-2] */
	short int *Filter,	/**< Pointer to Filter coeffs, */
	short int Bias,		/**< Bias input value, relevant only in 1 output mode */
	unsigned int W,		/**< Input tile width */
	unsigned int H,		/**< Input tile height */
	unsigned int OutMask	/**< Number of output produced: 0x7 => 1 out, 0x3 => 2 out, 0x1 => 3 out  */
	);

/** @brief HWCE based 5x5 convolution, short inputs, outputs, coefficients and bias.

HWCE based 5x5 convolution, short inputs, outputs, coefficients and bias.

        5x5, 1 output per cycle
        HWCE 5x5 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output
             is produced at a time
        Filter:
             (5*5 + Pad) * Nip * Nop coefficients
             Layout is:
                     <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                     <Coeff Group for OutPlanei> =
                             <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                     <Coeffs for InPlanej> = Cij0 Cij1 ... Cij24 0

        Bias[Nop]: Bias to be used as an init value for each output of the outplane set
        Norm: fixed point precision

        Convolution output is W-4 x H-4 (no padding).
        Warning:
             W has to be a multiple of 2
             Each filter group (5x5) should be padded with one zero, this is a HWCE alignment constraint
*/
void HWCE_ProcessOneTile5x5(
	short int *In,		/**< Pointer to input, Dim: [W x H], max Q15. */
	short int *Out,		/**< Pointer to output, Dim: [W-4 x H-4], max Q15. */
	short int *Filter,	/**< Pointer to convolution coeffs, Dim: [5x5] + 1, Max Q15 */
	short int Bias,		/**< Input Bias */
	unsigned int W,		/**< Input width */
	unsigned int H		/**< Input height */
	);

/** @brief HWCE based 7x7 convolution, short inputs, outputs, coefficients and bias.

HWCE based 7x7 convolution, short inputs, outputs, coefficients and bias.

        7x7, 1 output each 2 cycles since it is built through combination of 2 adjacent 7x4 convolutions.
	The second convolution has it's last line at 0.
        HWCE 7x7 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output.
        is produced at a time
        Filter:
             (7*8) * Nip * Nop coefficients, last line is all 0
             Layout is:
                     <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                     <Coeff Group for OutPlanei> =
                             <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                     <Coeffs for InPlanej> = Cij0 Cij1 ... Cij48 0 0 0 0 0 0 0
        Bias[Nop]: Bias to be used as an init value for each output of the outplane set
        Norm: fixed point precision

        Convolution output is W-6 x H-6 (no padding).
        Warning:
             W has to be a multiple of 2
             Each filter group (7x7) should be padded with 7 zero
*/
void HWCE_ProcessOneTile7x7(
	short int *In,		/**< Pointer to input, Dim: [W x H], max Q15. */
	short int *Out,		/**< Pointer to output, Dim: [W-6 x H-6], max Q15. */
	short int *Filter,	/**< Pointer to convolution coeffs, Dim: [7 x 8], last line is all 0. */
	short int Bias,		/**< Input bias */
	unsigned int W,		/**< Input width */
	unsigned int H		/**< Input height */
	);

/** @brief HWCE based 7x4 convolution, short inputs, outputs, coefficients and bias.

HWCE based 7x4 convolution, short inputs, outputs, coefficients and bias.

        7x4, 1 output per cycle
        HWCE 7x4 convolution on Nip input planes, each of them is W x H, and Nop output planes. One convolution output.
        is produced at a time
        Filter:
             (7*4) * Nip * Nop coefficients
             Layout is:
                     <Coeff Group for OutPlane0><Coeff Group for OutPlane1> ... <Coeff Group for OutPlaneNop-1>
                     <Coeff Group for OutPlanei> =
                             <Coeffs for InPlane0><Coeffs for InPlane1> ... <Coeffs for InPlaneNip-1>
                     <Coeffs for InPlanej> = Cij0 Cij1 ... Cij27
        Bias[Nop]: Bias to be used as an init value for each output of the outplane set
        Norm: fixed point precision

        Convolution output is W-6 x H-3 (no padding).
        Warning:
             W has to be a multiple of 2
*/
void HWCE_ProcessOneTile7x4(
	short int *In,		/**< Pointer to input, Dim: [W x H], max Q15. */
	short int *Out,		/**< Pointer to output, Dim: [W-6 x H-3], max Q15. */
	short int *Filter,	/**< Pointer to convolution coeffs, Dim: [7 x 4]. */
	short int Bias,		/**< Input bias */
	unsigned int W,		/**< Input width */
	unsigned int H		/**< Input height */
	);

/** @} */ // End of CNNHWCEBasicKernels
/** @} */

#endif
