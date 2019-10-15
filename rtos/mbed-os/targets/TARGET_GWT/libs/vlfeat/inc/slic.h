/******************************************************************
 *				FUNNY STUFF HERE
 *
 * Author:	Daniele Palossi
 * Email: 	daniele.palossi@iis.ee.ethz.ch
 *
 ******************************************************************/


#ifndef SLIC_H
#define SLIC_H

#include "generic.h"
#include "generic_slic.h"
#include "generic_hog.h"
#include "fixedptc.h"

/* Define if you want to enable OMP */
#define OMP
/* Define the number of OMP thread used */
#ifndef OMP
#define NUM_THDS 1	/* Do not change this! */
#else
#define NUM_THDS 8	/* Select your configuration*/
#endif
/* Define the segmentation temporary buffer for each thread */
#define TEMP_SEG_BUFFER 50
/* Defined if you want evaluate the checksums */
#define CHECKSUMS
/* Defined if you want STRIDED version */
// #define STRIDED

#ifdef CHECKSUMS
extern int checksums[5] ;
	#ifndef PROFILING_LIB
		//#define LOG_GRADIENT
		#define CHECKSUM_GRADIENT 6816888
		//#define LOG_CENTERS
		#define CHECKSUM_CENTERS 32894
		//#define LOG_KMEANS
		#define CHECKSUM_KMEANS 21924
		//#define LOG_SEGMENTATION
		#define CHECKSUM_SEGMENTATION 141040
		//#define LOG_SEGMENTATION_FINAL
		#define CHECKSUM_SEGMENTATION_FINAL 139848
	#endif
#endif

typedef struct raw_image_data {
    unsigned short 	*data;
    unsigned int  	width;
    unsigned int	height;
    unsigned int	nr_channels;
} raw_image_data_t;


extern fixedpt *centers ;
extern fixedpt *masses ;

void vl_slic_segment_strided(unsigned char * segmentation,
							 unsigned char * image,
							 vl_size width,
							 vl_size height,
							 vl_size numChannels,
							 vl_size regionSize,
							 vl_size regularization,
							 vl_size minRegionSize,
							 vl_size iteration);

void vl_slic_segment(unsigned char * segmentation,
					 unsigned char * image,
					 vl_size width,
					 vl_size height,
					 vl_size numChannels,
					 vl_size regionSize,
					 vl_size regularization,
					 vl_size minRegionSize);

#endif

