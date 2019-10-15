/******************************************************************
 *				FUNNY STUFF HERE
 *
 * Author:	Daniele Palossi
 * Email: 	daniele.palossi@iis.ee.ethz.ch
 *
 *NOTES:
 *. la regola e' che ci vogliono 2 fascie di regioni per poter far uscire in pout la precedente
 *
 *
 ******************************************************************/

#include "slic.h"
#include "data.h"
#include "common.h"

#define ITERS        3U
/* #define CHECK */

void fake_DMA(unsigned char *src, unsigned char *dst, unsigned int size) {
	for (int i=0; i<size; i++) {
		dst[i] = src[i];
	}
}

int main(int argc, char **argv) {

    /* printf("SLIC START\n"); */

	vl_set_mem_type(L1) ;

	/* ******************** STRIPE SIZE ******************** *
	 *  ___________________________________________________  *
	 * |_|_______________________________________________|_| *
	 * | |												 | | *
	 * | |	REGIONs [0:N]								 | | *
	 * |_|_______________________________________________|_| *
	 * | |												 | | *
	 * | |	REGIONs [1:N]								 | | *
	 * |_|_______________________________________________|_| *
	 * | |												 | | *
	 * | |	REGIONs [2:N]								 | | *
	 * |_|_______________________________________________|_| *
	 * |_|_______________________________________________|_| *
	 * 														 *
	 * (3xregionSize + 2 pixels) x IMG_WIDTH				 *
	 * ***************************************************** */

	int i, time 						= 0 ;

	vl_size input_width      			= IMG_WIDTH ;
	vl_size input_height     			= IMG_HEIGHT ;
	vl_size input_pixels    			= N ;
	vl_size input_channels  			= 1 ;
	vl_size regionSize       			= 5 ;
	vl_size regularization   			= 100 ;
	vl_size minRegionSize    			= 5 ;
	vl_size first_regionStripe_in			= 3 ;
	vl_size regionStripe_in			= 2 ;
	vl_size regionStripe_out			= first_regionStripe_in+1 ;				// 4
	vl_size first_stripe_height			= regionSize*first_regionStripe_in ;	// 15
	vl_size stripe_height				= regionSize*regionStripe_in ;			// 10
	vl_size first_stripe_size			= (first_stripe_height+1)*input_width ;	// (15+1)*63
	vl_size stripe_size				= (stripe_height+2)*input_width ;		// (10+2)*63
	vl_size dma_in_size				= 0 ;
	vl_size remaining_pixels			= N ;
	vl_size nb_iteration				= 1+fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(input_height-first_stripe_height), fixedpt_fromint(stripe_height)))) ;

//	printf("nb_iter: %d\n", nb_iteration);

	unsigned char *input_img 			= NULL ;
	unsigned char *segmentation 		= NULL ;
	unsigned char *data[2] 				= {NULL, NULL} ;
	unsigned char *buffer_in_dma 		= NULL ;
	unsigned char *buffer_in_compute	= NULL ;

	dma_req_t req = {-1};

	/* Input Buffers Allocation */
#ifdef STRIDED
	input_img = data_img ;
	data[0] = vl_malloc(first_stripe_size*sizeof(unsigned char)) ;		// 714 Byte
	data[1] = vl_malloc(first_stripe_size*sizeof(unsigned char)) ;		// 714 Byte
	buffer_in_dma = data[0] ;

	/* Output Buffer Allocation */
	segmentation = vl_malloc((regionStripe_out*regionSize) * input_width * sizeof(unsigned char)) ; // 1008 Byte
#else
	input_img = vl_malloc(input_pixels * sizeof(unsigned char)) ;	// 2646 Byte
	fake_DMA(data_img, input_img, input_pixels) ;

	/* Output Buffer Allocation */
	segmentation = vl_malloc(input_pixels * sizeof(unsigned char)) ; // 2646 Byte
#endif



    for(int iter = 0; iter < ITERS; ++iter)
    {
        profile_start(iter);

#ifdef STRIDED
	/* Start Iteration */
	for(i=0; i<nb_iteration+1; i++) {

		/* Double Buffering Switching */
		buffer_in_compute = buffer_in_dma ;
		if (buffer_in_dma == data[0])
			buffer_in_dma = data[1] ;
		else
			buffer_in_dma = data[0] ;

		/* DMA Input Copy */
		if(i<nb_iteration) {
			if(i==0) dma_in_size = first_stripe_size ;
			else dma_in_size = MIN(stripe_size, remaining_pixels) ;

                        DMAMCHAN_Memcpy_1D((uint32_t)input_img, (uint32_t)buffer_in_dma, dma_in_size, GAP_DMA_L22TCDM, &req);

			if(i==0) {
				input_img += (first_stripe_height-2)*input_width ;
				remaining_pixels -= (first_stripe_height-2)*input_width ;
			}
			else {
				input_img += (stripe_height-2)*input_width ;
				remaining_pixels -= (stripe_height-2)*input_width ;
			}
		}

		/* SLIC Computation */
		if(i>0) {
			vl_size width_compute	= input_width ;
			vl_size height_compute ;
			if(i==1) height_compute	= first_stripe_height ;
			else height_compute		= MIN(stripe_height+2, input_height - (first_stripe_height-2 + ((i-2)*(stripe_height)))) ;

			vl_slic_segment_strided(segmentation,
									buffer_in_compute,
									width_compute,
									height_compute,
									input_channels,
									regionSize,
									regularization,
									minRegionSize, i-1);
		}

		/* DMA Input Wait */
		if(i<nb_iteration) {
                    if (req.tid != -1) {
                        DMAMCHAN_WaitRequestEnd(&req);
                        req.tid = -1;
                    }
		}


	} /*End Iteration*/

#else
	/* NOTE: MAX IMAGE SIZE 256x256 PIXEL!!! */
	vl_slic_segment(segmentation,
					input_img,
					input_width,
					input_height,
					input_channels,
					regionSize,
					regularization,
					minRegionSize);
#endif

        profile_stop(iter);

#ifdef STRIDED
	#ifdef CHECK
	  if(checksums[0] == CHECKSUM_GRADIENT)
		  printf("CHECKSUM GRADIENT:\t\tSUCCESS [%d vs %d]\n", checksums[0], CHECKSUM_GRADIENT) ;
	  else
		  printf("CHECKSUM GRADIENT:\t\tERROR [%d vs %d]\n", checksums[0], CHECKSUM_GRADIENT) ;

	  if(checksums[1] == CHECKSUM_CENTERS)
	  	  printf("CHECKSUM CENTERS:\t\tSUCCESS [%d vs %d]\n", checksums[1], CHECKSUM_CENTERS) ;
	    else
	  	  printf("CHECKSUM CENTERS:\t\tERROR [%d vs %d]\n", checksums[1], CHECKSUM_CENTERS) ;

	  if(checksums[2] == CHECKSUM_KMEANS)
		  printf("CHECKSUM K-MEANS:\t\tSUCCESS [%d vs %d]\n", checksums[2], CHECKSUM_KMEANS) ;
	  else
		  printf("CHECKSUM K-MEANS:\t\tERROR [%d vs %d]\n", checksums[2], CHECKSUM_KMEANS) ;


	 #endif
#endif
    }
    profile_show();

    return 0;
}
