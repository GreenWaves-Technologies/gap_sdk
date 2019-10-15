/** @file hog.h
 ** @brief Histogram of Oriented Gradients (@ref hog)
 ** @author Andrea Vedaldi
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
*/

/******************************************************************
 *				FUNNY STUFF HERE
 *
 * Author:	Daniele Palossi
 * Email: 	daniele.palossi@iis.ee.ethz.ch
 *
 ******************************************************************/

#ifndef VL_HOG_H
#define VL_HOG_H

#define OMP
#define NUM_THDS 8
#define MEM_DYN
#define APPROX

#include "host.h"
#include "generic.h"
#include "generic_hog.h"
#include "fixedptc.h"


struct VlHog_
{
//	VlHogVariant variant ;
	vl_size dimension ;
	vl_size numOrientations ;
	vl_bool transposed ;
	vl_bool useBilinearOrientationAssigment ;

	/* left-right flip permutation */
	vl_index * permutation ;

	/* glyphs */
	fixedpt * glyphs ;
	vl_size glyphSize ;

	/* helper vectors */
	fixedpt * orientationX ;
	fixedpt * orientationY ;

	/* buffers */
	fixedpt * hog ;
	fixedpt * hogNorm ;
	vl_size hogWidth ;
	vl_size hogHeight ;
} ;

typedef struct VlHog_ VlHog ;

/* Dynamic memory version */
#ifdef MEM_DYN
VL_EXPORT VlHog * vl_hog_new (			vl_size 	numOrientations,
										vl_bool 	transposed) ;
#else
/* Static memory version */
VL_EXPORT void vl_hog_new (				vl_size 	numOrientations,
										vl_bool 	transposed,
										VlHog 		*self) ;
#endif


VL_EXPORT void vl_hog_delete (			VlHog 		*self) ;

VL_EXPORT void vl_hog_put_image (		VlHog 		*self,
										unsigned char	*image,
										vl_size 	width,
										vl_size 	height,
										vl_size 	numChannels,
										vl_size 	cellSize,
										vl_size 	iteration) ;

VL_EXPORT void vl_hog_extract (			VlHog 		*self,
										fixedpt		*features,
										vl_index 	iteration) ;

VL_EXPORT vl_size vl_hog_get_height (	VlHog 		*self) ;

VL_EXPORT vl_size vl_hog_get_width (	VlHog 		*self) ;

VL_EXPORT vl_size vl_hog_get_dimension (VlHog const *self) ;

VL_EXPORT void hog_accuracy_eval(float *ref, fixedpt *test, int blocksOverlaps, int numOrientations, int hogWidth, int hogHeight, int verbose) ;


//VL_EXPORT void vl_hog_process (VlHog *self, float *features, float const *image, vl_size width, vl_size height, vl_size numChannels, vl_size cellSize) ;
//VL_EXPORT void vl_hog_put_polar_field (VlHog *self, float const *modulus, float const *angle, vl_bool directed, vl_size width, vl_size height, vl_size cellSize) ;
//VL_EXPORT void vl_hog_render (VlHog const *self, float *image, fixedpt const *features, vl_size width, vl_size height) ;
//VL_EXPORT vl_index const * vl_hog_get_permutation (VlHog const * self) ;
//VL_EXPORT vl_size vl_hog_get_glyph_size (VlHog const * self) ;
//VL_EXPORT vl_bool vl_hog_get_use_bilinear_orientation_assignments (VlHog const * self) ;
//VL_EXPORT void vl_hog_set_use_bilinear_orientation_assignments (VlHog * self, vl_bool x) ;

/* VL_HOG_H */
#endif
