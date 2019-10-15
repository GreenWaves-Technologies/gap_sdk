/******************************************************************
 *				FUNNY STUFF HERE
 *
 * Author:	Daniele Palossi
 * Email: 	daniele.palossi@iis.ee.ethz.ch
 *
 ******************************************************************/

#include "hog.h"
#include "common.h"
#include "data.h"

/* CHURCH 128x128 */
/* #ifdef APPROX */
/* #define CHECKSUM1	1377628496 */
/* #define CHECKSUM_NORM	1310079360 */
/* #define CHECKSUM2	102834841 */
/* #else */
/* #define CHECKSUM1	1382152667 */
/* #define CHECKSUM_NORM	1318352119 */
/* #define CHECKSUM2	102862885 */
/* #endif */

#define ACCURACY_EVAL

#ifdef ACCURACY_EVAL
#include "hog_reference_church_128_128.h"
fixedpt *final_descriptor ;
fixedpt *final_descriptor_ptr ;
#endif

void fake_DMA(unsigned char *src, unsigned char *dst, unsigned int size) {
    for (int i=0; i<size; i++) {

        dst[i] = src[i];
    }
}

#ifdef CHECKSUM1
unsigned long int check1 = 0 ;
#endif
#ifdef CHECKSUM_NORM
unsigned long int check_norm = 0 ;
#endif
#ifdef CHECKSUM2
unsigned long int check2 = 0 ;
#endif

int main(int argc, char **argv) {
	vl_set_mem_type(L1) ;

	int time = 0 ;
	int n = N;
        int i, j, k;
	unsigned char *img_ptr = data_img ;
	unsigned char *data[2] ;
	fixedpt *hogArray = NULL ;
	vl_size numOrientations = 9;
	vl_size numChannels = 1;
	vl_size cellSize = 8;
	vl_size hogDimension = numOrientations * 4 ;
	vl_size num_threads = NUM_THDS;//omp_get_max_threads() ;
	vl_size stripe_elements_1st = VL_MAX(num_threads*2, 3) ;
	vl_size hog_buff_elements = stripe_elements_1st+1;


	VlHog * hog = vl_hog_new(numOrientations, VL_FALSE) ;

	vl_size hog_total_width = (IMG_WIDTH + cellSize/2) / cellSize ;
	vl_size hog_total_height = (IMG_HEIGHT + cellSize/2) / cellSize ;
	vl_size nb_it = fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(hog_total_height), fixedpt_fromint(NUM_THDS)))) ;
	vl_size hog_single_elements_buffer_size = numOrientations*2*hog_total_width ;
	vl_size hog_buff_size = hog_single_elements_buffer_size*hog_buff_elements ;

	unsigned short int stripe_size_1st = (stripe_elements_1st*cellSize+2)*IMG_WIDTH ;
	unsigned short int stripe_size = (num_threads*cellSize+2)*IMG_WIDTH ;
	vl_size dma_in_data_size[2] ;
	vl_size *dma_in_size = &dma_in_data_size[0] ;// = stripe_size_1st ;
	vl_size prev_dma_in_size ;

	/* ****************** VERY IMPORTANT NOTE ***************** *
	 * The first input buffer used should be initialized at:	*
	 * stripe_size_1st*sizeof(unsigned char)+8 Bytes.			*
	 * But due to the presence as next buffer in memory of		*
	 * hogArray, we can use a smaller size for the two data		*
	 * input buffers and have a "controlled overflow" at the	*
	 * first iteration. This can be safely done because the		*
	 * hogArray is required starting from the second iteration	*
	 * only after the first input buffer has been consumed.		*
	 *															*
	 * Buffers in Memory:										*
	 * ------------------------------------------				*
	 * |  data[0]	|  data[1]	|	hogArray	|				*
	 * ------------------------------------------				*
	 * 		4KB			4KB				9KB						*
	 * ******************************************************** */
	data[0] = vl_malloc(stripe_size*sizeof(unsigned char)+8) ; 						// 4360 B + 8 B
	data[1] = vl_malloc(stripe_size*sizeof(unsigned char)+8) ; 						// 4360 B + 8 B

	unsigned char *buffer_in_dma = (unsigned char *) data[0] ;
	unsigned char *buffer_in_compute ;

	hogArray = vl_malloc(num_threads*hog_total_width*hogDimension*sizeof(fixedpt)) ;

#ifdef ACCURACY_EVAL
	/* Memory Allocation for the final descriptor */
	final_descriptor = vl_malloc(BLOCKS_OVERLAP*NUM_ORIENTATION*NUM_CELLS*sizeof(fixedpt)) ; // 36864 Byte
	final_descriptor_ptr = final_descriptor ;
#endif


    for(int iter = 0; iter < 2; ++iter)
    {
        profile_start(iter);

	int skip = 0 ;
	int counter_buffer = hog_total_height;
	dma_req_t req = {-1};

	for(i=0; i<nb_it+1; i++) /* stripe iteration */ {

		buffer_in_compute = buffer_in_dma ;
		if (i%2 == 0)
			buffer_in_dma = data[1] ;
		else
			buffer_in_dma = data[0] ;

		prev_dma_in_size = *dma_in_size;
		if (dma_in_size == &dma_in_data_size[0])
			dma_in_size =  &dma_in_data_size[1] ;
		else
			dma_in_size = &dma_in_data_size[0] ;

		if(i == 0) *dma_in_size = stripe_size_1st ;
		else *dma_in_size = MIN(stripe_size, n) ;

		/* DMA INPUT COPY */
		if(n > 2*IMG_WIDTH) {
                    DMAMCHAN_Memcpy_1D((uint32_t)img_ptr, (uint32_t)buffer_in_dma, *dma_in_size, GAP_DMA_L22TCDM, &req);
		}

		/* HOG Stage 1*/
		if(i>0 && !skip) {

			vl_hog_put_image(hog, buffer_in_compute, IMG_WIDTH, prev_dma_in_size/IMG_WIDTH, numChannels, cellSize, i-1) ;

#if CHECKSUM1
			/* PRINT the temporary HOG buffer New style*/
			int off = 0;
			if(i-1 != 0) off = hog_single_elements_buffer_size*(((stripe_elements_1st-1)+((i-2)*num_threads))%hog_buff_elements) ;

			/* First iteration */
			if(i-1 == 0) {
				for(j=0; j<hog_single_elements_buffer_size*(stripe_elements_1st-1); j++) check1 += hog->hog[(j+off)%hog_buff_size] ;
				counter_buffer -= stripe_elements_1st-1;
			}
			else {
				int it ;
				if(counter_buffer == num_threads+1) it = num_threads+1; // Last Iteration for thds 1,2,4
				else it = VL_MIN(counter_buffer,num_threads) ;
				for(j=0; j<hog_single_elements_buffer_size*it; j++) check1 += hog->hog[(j+off)%hog_buff_size] ;
				counter_buffer -= it;
			}
#endif
		}

		/* HOG Stage 2*/
		if(i>0) {
			vl_hog_extract(hog, hogArray, i-1) ;

#ifdef CHECKSUM2
		int it = VL_MIN(num_threads, hog_total_height-(i-1)*num_threads) ;
		for(j=0; j<hog_total_width*hogDimension*it; j++) check2 += hogArray[j];
#endif

#ifdef ACCURACY_EVAL
		/* Slice-copy of the final descriptor */
		int it2 = VL_MIN(num_threads, hog_total_height-(i-1)*num_threads) ;
		int copy_size = hog_total_width*hogDimension*it2 ;
		fake_DMA(hogArray, final_descriptor_ptr, copy_size*sizeof(fixedpt)) ;
		final_descriptor_ptr += copy_size ;
#endif

		/* PRINT the HOG array */
		/* if(i==0) for(j=0; j<16*9*4*4; j++) qprintf("%d\n", hogArray[j]); */
		/* if(i==1) for(j=0; j<16*9*4*4; j++) qprintf("%d\n", hogArray[j]); */
		/* if(i==2) for(j=0; j<16*9*4*4; j++) qprintf("%d\n", hogArray[j]); */
		/* if(i==3) for(j=0; j<16*9*4*4; j++) qprintf("%d\n", hogArray[j]); */
		}

		// TODO: introduce else
		if(n<=2*IMG_WIDTH) {
			skip = 1 ;
		}

		if(n > 2*IMG_WIDTH) {
                    img_ptr += (*dma_in_size - 2*IMG_WIDTH) ;
                    n -= (*dma_in_size - 2*IMG_WIDTH) ;
		}

		if (req.tid != -1) {
                    DMAMCHAN_WaitRequestEnd(&req);
                    req.tid = -1;
                }
	}  /* close stripe iteration */

        profile_stop(iter);

#ifdef CHECKSUM_NORM
        for(j=0; j<hog_total_width*hog_total_height; j++) check_norm +=  hog->hogNorm[j];
#endif

	/* PRINT the HOG Norm */
	/* for(k=0; k<hog_total_width*hog_total_height; k++) qprintf("%d\n", hog->hogNorm[k]); */

	vl_hog_delete(hog) ;

    }
    profile_show();

#ifdef CHECKSUM1
	if (check1 == CHECKSUM1)
            printf("CHECKSUM 1:\t\tSUCCESS\t\t[%d %d]\n", (int)check1, (int)CHECKSUM1);
	else
            printf("CHECKSUM 1:\t\tERROR\t\t[%d %d]\n", (int)check1, (int)CHECKSUM1);
#endif
#ifdef CHECKSUM_NORM
	if (check_norm == CHECKSUM_NORM)
            printf("CHECKSUM NORMALS:\tSUCCESS\t\t[%d %d]\n", (int)check_norm, (int)CHECKSUM_NORM);
	else
            printf("CHECKSUM NORMALS:\tERROR\t\t[%d %d]\n", (int)check_norm, (int)CHECKSUM_NORM);
#endif
#ifdef CHECKSUM2
	if (check2 == CHECKSUM2)
            printf("CHECKSUM 2:\t\tSUCCESS\t\t[%d %d]\n", (int)check2, (int)CHECKSUM2);
	else
            printf("CHECKSUM 2:\t\tERROR\t\t[%d %d]\n", (int)check2, (int)CHECKSUM2);
#endif


#ifdef ACCURACY_EVAL
	/* Final Accuracy Evaluation*/
	hog_accuracy_eval(ref,
                          final_descriptor,
                          BLOCKS_OVERLAP,
                          NUM_ORIENTATION,
                          HOG_WIDTH_REF,
                          HOG_HEIGHT_REF,
                          0) ;
#endif

        return 0;

}
