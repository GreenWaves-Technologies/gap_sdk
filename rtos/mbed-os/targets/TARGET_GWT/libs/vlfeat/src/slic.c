/******************************************************************
 *				FUNNY STUFF HERE
 *
 * Author:	Daniele Palossi
 * Email: 	daniele.palossi@iis.ee.ethz.ch
 *
 ******************************************************************/



#include "slic.h"

#ifdef CHECKSUMS
int checksums[] = {0,0,0,0,0} ;
#endif

int a = 800;
int b = 900;

void vl_slic_segment_strided(unsigned char * segmentation,  // (5*3+1)*63 pixel
                     	 	 unsigned char * image,			// (5*2+2)*63 pixel
							 vl_size width,					// 63 pixel
							 vl_size height,				// 5*2+2 pixel
							 vl_size numChannels,			// 1
							 vl_size regionSize,			// 5
							 vl_size regularization,		// 100
							 vl_size minRegionSize,			// 5
							 vl_size iteration) {

	vl_index i, x, y, u, v, k, region ;
	vl_uindex iter ;
	vl_size const numRegionsX = (vl_size) fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(width), fixedpt_fromint(regionSize))));
	vl_size const numRegionsY = (vl_size) fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(height), fixedpt_fromint(regionSize))));
	vl_size const numRegions = numRegionsX * numRegionsY ;
	vl_size const numPixels = width * height ;
	fixedpt previousEnergy = INT_MAX ;
	fixedpt startingEnergy ;
	vl_size const maxNumIterations = 100 ;
	extern int *edgeMap ;

//	printf("%d %d\n",numRegionsX,numRegionsY) ;

	/* VL_MALLOC allocations */
	edgeMap = vl_calloc(numPixels, sizeof(int)) ;
	masses = vl_malloc(sizeof(fixedpt) * (2 + numChannels) * numRegions * NUM_THDS) ;
	centers = vl_malloc(sizeof(fixedpt) * (2 + numChannels) * numRegions * NUM_THDS) ;


/* ============================== STAGE 1: COMPUTE EDGE MAP (GRADIENT STRENGHT) ==============================*/

	/* compute edge map (gradient strength) */
	for(k = 0 ; k < (signed)numChannels ; ++k) {
		int k_0 = k*numPixels ;

#ifdef OMP
#pragma omp parallel for default (none) \
shared(image,edgeMap) \
private(x) \
firstprivate(height,width,numChannels,k_0) \
num_threads(NUM_THDS)
#endif
		for(y = 1 ; y < (signed)height-1 ; ++y) {

			int y_0 = y 	*  	width ;
			int y_1 = (y+1) * 	width ;
			int y_2 = (y-1) * 	width ;

			for(x = 1 ; x < (signed)width-1 ; ++x) {
				unsigned char a = image[(x-1)	+ y_0	+ k_0] ;
				unsigned char b = image[(x+1)	+ y_0	+ k_0] ;
				unsigned char c = image[x		+ y_1	+ k_0] ;
				unsigned char d = image[x		+ y_2	+ k_0] ;
				edgeMap[x+y_0] += (a - b) * (a - b) + (c - d) * (c - d) ;
			}
		}
	}


#ifdef LOG_GRADIENT
	for(i=0; i<numPixels; i++)
		printf("%d\n", edgeMap[i]) ;
#endif

#ifdef CHECKSUM_GRADIENT
  for(i=0; i<numPixels; i++)
	  checksums[0] += edgeMap[i] ;
#endif


/* =================================== STAGE 2: INITIALIZE K-MEANS CENTERS ===================================*/

#ifdef OMP
#pragma omp parallel for default (none) \
shared(edgeMap,image,centers) \
private(i,u,x,y,k) \
firstprivate(numRegionsY,numRegionsX,regionSize,width,height,numChannels) \
num_threads(NUM_THDS)
#endif
	for(v = 0; v < (signed)numRegionsY; ++v) {
		for(u = 0; u < (signed)numRegionsX; ++u) {
			vl_index xp ;
			vl_index yp ;
			int centerx = 0 ;
			int centery = 0 ;
			int minEdgeValue = INT_MAX ;

			x = fixedpt_toint(fixedpt_round(fixedpt_mul(fixedpt_fromint(regionSize), (fixedpt_fromint(u) + FIXEDPT_ONE_HALF))));
			y = fixedpt_toint(fixedpt_round(fixedpt_mul(fixedpt_fromint(regionSize), (fixedpt_fromint(v) + FIXEDPT_ONE_HALF))));
//if(u==0 && v==0) printf("%d %d\n",x,y); //3 3

			x = VL_MAX(VL_MIN(x, (signed)width-1),0) ;
			y = VL_MAX(VL_MIN(y, (signed)height-1),0) ;

			/* search in a 3x3 neighborhood the smallest edge response centered on the region center*/
			for(yp = VL_MAX(0, y-1) ; yp <= VL_MIN((signed)height-1, y+1) ; ++ yp) {
				for(xp = VL_MAX(0, x-1) ; xp <= VL_MIN((signed)width-1, x+1) ; ++ xp) {
//					if(u==1 && v==0) printf("%d %d\n",xp,yp);
					int thisEdgeValue = edgeMap[(xp)+(yp)*width];
					if (thisEdgeValue < minEdgeValue) {
						minEdgeValue = thisEdgeValue ;
						centerx = xp ;
						centery = yp ;
					}
				}
			}

			/* initialize the new center at this location */
			i = (u + v*numRegionsX)*3 ;
			centers[i++] = fixedpt_fromint(centerx) ;
			centers[i++] = fixedpt_fromint(centery) ;
//			printf("%d %d\n",centerx,centery) ;

			for (k  = 0 ; k < (signed)numChannels ; ++k) {
				centers[i++] = fixedpt_fromint((int)image[(centerx)+(centery)*width+(k)*numPixels]);
//				printf("%d\n",image[(centerx)+(centery)*width+(k)*numPixels]) ;
			}
		}
	}

#ifdef LOG_CENTERS
  for(i=0; i<((2 + numChannels) * numRegions); i++)
		fixedpt_print(centers[i]) ;
#endif

#ifdef CHECKSUM_CENTERS
  for(i=0; i<((2 + numChannels) * numRegions); i++)
	  checksums[1] += fixedpt_toint(centers[i]) ;
#endif


/* ===================================== STAGE 3: RUN K-MEANS ITERATIONS =====================================*/

  	fixedpt 	one_over_regionSize = fixedpt_div(FIXEDPT_ONE, fixedpt_fromint(regionSize)) ;
  	fixedpt		ten_thousand 		= fixedpt_frominttoHR(100000) ;
  	vl_index 	*u_array 			= vl_malloc((width+regionSize)*sizeof(vl_index)) ;
  	vl_index 	*v_array 			= vl_malloc((height+regionSize)*sizeof(vl_index)) ;
  	fixedpt 	factor 				= fixedpt_div(fixedpt_fromint(regularization), fixedpt_fromint(regionSize * regionSize)) ;
    int off 						= (2 + numChannels)*numRegions ;
  	fixedpt 	*masses_ptr ;
  	fixedpt 	*centers_ptr ;

#ifdef OMP
#pragma omp parallel for default(none) shared(v_array,u_array) private(x) firstprivate(height,width,one_over_regionSize) num_threads(NUM_THDS)
#endif
    for (y = 0 ; y < (signed)height ; y++)
    	v_array[y] = fixedpt_toint(fixedpt_floor(fixedpt_mul(fixedpt_fromint(y),one_over_regionSize) - FIXEDPT_ONE_HALF));

#ifdef OMP
        #pragma omp parallel for default(none) shared(v_array,u_array) private(y) firstprivate(height,width,one_over_regionSize) num_threads(NUM_THDS)
#endif
    for (x = 0 ; x < (signed)width ; x++)
    	u_array[x] = fixedpt_toint(fixedpt_floor(fixedpt_mul(fixedpt_fromint(x),one_over_regionSize) - FIXEDPT_ONE_HALF));

#ifdef LOG_KMEANS
#endif

#ifdef CHECKSUM_KMEANS
	int checksum_kmeans_u = 0 ;
	int checksum_kmeans_v = 0 ;

	for(i=0; i<width; i++)
		checksum_kmeans_u += u_array[i] ;
	for(i=0; i<height; i++)
		checksum_kmeans_v += v_array[i] ;

	checksums[2] += (checksum_kmeans_v * width)+ (checksum_kmeans_u * height);
#endif

	for (iter = 0 ; iter < maxNumIterations ; ++iter) {

		fixedpt *energy_array = vl_calloc(NUM_THDS, sizeof(fixedpt)) ;
		fixedpt energy = 0 ;

#ifdef OMP
#pragma omp parallel for default (none) \
shared(u_array,v_array,centers,image,segmentation,energy_array) \
private(x,k) \
firstprivate(height,width,iter,factor,numChannels) \
num_threads(NUM_THDS)
#endif
		/* assign pixels to centers */
		for (y = 0 ; y < (signed)height ; ++y) {

			int thd_id =
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
			for (x = 0 ; x < (signed)width ; ++x) {

				vl_index up, vp ;
				vl_index pixel = x + y * width ;
				fixedpt minDistance = INT_MAX ;
				vl_index u = u_array[x] ;
				vl_index v = v_array[y] ;
//				if(iter==0 && iteration==0) printf("%d %d - %d %d\n",x,y,u,v);

				for (vp = VL_MAX(0, v) ; vp <= VL_MIN((signed)numRegionsY-1, v+1) ; ++vp) {
					for (up = VL_MAX(0, u) ; up <= VL_MIN((signed)numRegionsX-1, u+1) ; ++up) {
						vl_index region = up  + vp * numRegionsX ;

						fixedpt centerx = centers[(2 + numChannels) * region + 0] ;
						fixedpt centery = centers[(2 + numChannels) * region + 1] ;
//						if(iter==0 && iteration==0 && pixel==10) printf("%d %d %d\n",centerx,centery,region) ;

						fixedpt spatial = fixedpt_mul((fixedpt_fromint(x) - centerx), (fixedpt_fromint(x) - centerx)) + fixedpt_mul((fixedpt_fromint(y) - centery), (fixedpt_fromint(y) - centery)) ;
						fixedpt appearance = 0 ;
						fixedpt distance ;
						for (k = 0 ; k < (signed)numChannels ; ++k) {
							fixedpt centerz = centers[(2 + numChannels) * region + k + 2]  ;
							fixedpt z = fixedpt_fromint((int)image[(x)+(y)*width+(k)*numPixels]);
							appearance += fixedpt_mul((z - centerz), (z - centerz)) ;
						}
						distance = appearance + fixedpt_mul(factor, spatial) ;
//						if(iter==0 && iteration==0) printf("%d %d\n",pixel,region) ;
//						if(iter==0 && iteration==0 && pixel==10) printf("%d\n",distance) ;

						if (minDistance > distance) {
							minDistance = distance ;
							segmentation[pixel] = (unsigned char) region ;
						}

//						if(iter==0 && iteration==0 && pixel>a && pixel<b)
//							printf("[%d %d] %d %d %d %d %d\n",pixel,region,vp,up,centerx,centery,distance) ;
					}
				}
				energy_array[thd_id] += fixedpt_to_HR(minDistance) ;
			}
		}

		for(i=0; i<NUM_THDS; i++)
			energy += energy_array[i];

		/* check energy termination conditions */
		if (iter == 0) {
			startingEnergy = energy ;
		} else {
			if ((previousEnergy - energy) < fixedpt_div_HR((startingEnergy - energy), ten_thousand)) {
				break ;
			}
		}
		previousEnergy = energy ;

	    /* recompute centers */
//	    memset(masses, 0, sizeof(fixedpt) * width * height) ;
//	    memset(centers, 0, sizeof(fixedpt) * (2 + numChannels) * numRegions) ;

//#pragma omp parallel for default (none) shared(masses) private(i) firstprivate(width,height) num_threads(NUM_THDS)
//	    for(i=0; i<width * height; i++)
//	    	masses[i] = 0 ;

#ifdef OMP
#pragma omp parallel for default (none) shared(centers,masses) firstprivate(numChannels,numRegions) num_threads(NUM_THDS)
#endif
		for(i=0; i<(2 + numChannels)*numRegions*NUM_THDS; i++) {
	    	centers[i] = 0 ;
	    	masses[i] = 0 ;
	    }

#ifdef OMP
#pragma omp parallel for default (none) \
shared(centers,image,masses,segmentation) \
private(x,k,masses_ptr,centers_ptr) \
firstprivate(height,width,numChannels,off) \
num_threads(NUM_THDS)
#endif
	    for (y = 0 ; y < (signed)height ; ++y) {

			masses_ptr = masses + off *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
			centers_ptr = centers + off *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif

	    	for (x = 0 ; x < (signed)width ; ++x) {
				vl_index pixel = x + y * width ;
				vl_index region = segmentation[pixel] ;
				masses_ptr[region] += FIXEDPT_ONE ;
//				if(iteration==0 && region==1) printf("%d) %d %d\n",iter,x,y) ;
				centers_ptr[region * (2 + numChannels) + 0] += x ;
				centers_ptr[region * (2 + numChannels) + 1] += y ;
				for (k = 0 ; k < (signed)numChannels ; ++k) {
					centers_ptr[region * (2 + numChannels) + k + 2] += (int)image[(x)+(y)*width+(k)*numPixels] ;
				}
	    	}
	    }

#ifdef OMP
#pragma omp parallel for default (none) shared(centers,masses) firstprivate(numChannels,numRegions,off) num_threads(NUM_THDS)
#endif
	    for(i=0; i<(2 + numChannels)*numRegions; i++) {
	    	int j ;
	    	for(j=1; j<NUM_THDS; j++) {
		    	masses[i] += masses[off*j+i] ;
		    	centers[i] += centers[off*j+i] ;
	    	}
	    }

#ifdef OMP
#pragma omp parallel for default (none) \
shared(centers,masses) \
private(i) \
firstprivate(numRegions,numChannels) \
num_threads(NUM_THDS)
#endif
		for (region = 0 ; region < (signed)numRegions ; ++region) {
			/* 1-e8, the smallest number in fixedpt is 1 = 0.000244140625 */
			fixedpt mass = MAX(masses[region], 1) ;
			for (	i = (2 + numChannels) * region ;
					i < (signed)(2 + numChannels) * (region + 1) ;
					++i) {
				centers[i] = fixedpt_div(fixedpt_fromint(centers[i]), mass) ;
			}
		}
	} // close Iter loop


if(iteration==0){
	for(y=0; y<5; y++)
	for(x=0; x<width; x++)
		printf("[%d %d] :: %d\n",x,y,segmentation[y*width+x]);
//		if(segmentation[y*width+x]<13) printf("[%d %d] :: %d\n",x,y,segmentation[y*width+x]);

//	printf("[%d %d] :: %d\n",x,y,segmentation[y*63+x]);
}


#ifdef LOG_SEGMENTATION
	for(i=0; i<numPixels; i++)
		printf("%d\n", segmentation[i]) ;
#endif

#ifdef CHECKSUM_SEGMENTATION
	int checksum_segmentation = 0 ;
	for(i=0; i<numPixels; i++)
		checksum_segmentation += segmentation[i] ;
	if(checksum_segmentation == CHECKSUM_SEGMENTATION)
		printf("CHECKSUM SEGMENTATION:\t\tSUCCESS [%d vs %d]\n", checksum_segmentation, CHECKSUM_SEGMENTATION) ;
	else
		printf("CHECKSUM SEGMENTATION:\t\tERROR [%d vs %d]\n", checksum_segmentation, CHECKSUM_SEGMENTATION) ;
#endif

	vl_free(masses) ;
	vl_free(centers) ;
	vl_free(edgeMap) ;

#if 0

/* ==================================== STAGE 4: ELIMINATE SMALL REGIONS =====================================*/

	unsigned char 	*cleaned = vl_calloc(numPixels, sizeof(unsigned char)) ;
//	vl_uint16 		*segment = vl_malloc(sizeof(vl_uint16) * numPixels * 2) ;
	vl_uint16 		*seg = vl_malloc(sizeof(vl_uint16) * TEMP_SEG_BUFFER * 2 * NUM_THDS) ;
	vl_size 		segmentSize ;
	unsigned char	label ;
	unsigned char 	cleanedLabel ;
	vl_size 		numExpanded ;
	vl_index const 	dx [] = {+1, -1,  0,  0} ;
	vl_index const 	dy [] = { 0,  0, +1, -1} ;
	vl_index 		direction ;
	vl_index 		pixel ;


//#pragma omp parallel for default (none) \
//shared(segmentation,cleaned,seg) \
//private(x,y,label,numExpanded,segmentSize,pixel,cleanedLabel,direction) \
//firstprivate(height,width,minRegionSize) \
//num_threads(NUM_THDS)

    for (y = 0 ; y < (signed)height ; ++y) {
    	for (x = 0 ; x < (signed)width ; ++x) {

    		vl_index pixel = x + y * width ;

			if (cleaned[pixel]) continue ;

			label = segmentation[pixel] ;
			numExpanded = 0;
			segmentSize = 0;
			vl_uint16 *segment = seg + 100 *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
//		    segment[segmentSize++] = pixel ;
			segment[segmentSize*2] = x ;
			segment[segmentSize*2+1] = y ;
			segmentSize++ ;

			/*
			 * find cleanedLabel as the label of an already cleaned
			 * region neighbor of this pixel
			 */

			cleanedLabel   = label + 1 ;
			cleaned[pixel] = label + 1 ;
//			x = pixel % width ;
//			y = pixel / width ;
			for (direction = 0 ; direction < 4 ; ++direction) {
				vl_index xp = x + dx[direction] ;
				vl_index yp = y + dy[direction] ;
				vl_index neighbor = xp + yp * width ;
				if (0 <= xp &&
					xp < (signed)width &&
					0 <= yp && yp < (signed)height &&
					cleaned[neighbor]) {
					cleanedLabel = cleaned[neighbor] ;
				}
			}

			/* expand the segment */
			while (numExpanded < segmentSize) {
//				vl_size open = segment[numExpanded++] ;
//				vl_index x_2 = open % width ;
//				vl_index y_2 = open / width ;
				vl_index x_2 = segment[numExpanded*2] ;
				vl_index y_2 = segment[numExpanded*2+1] ;
				numExpanded++ ;
				for (direction = 0 ; direction < 4 ; ++direction) {
					vl_index xp = x_2 + dx[direction] ;
					vl_index yp = y_2 + dy[direction] ;
					vl_index neighbor = xp + yp * width ;
					if (	0 <= xp && xp < (signed)width &&
							0 <= yp && yp < (signed)height &&
							cleaned[neighbor] == 0 &&
							segmentation[neighbor] == label) {
						cleaned[neighbor] = label + 1 ;
//						segment[segmentSize++] = neighbor ;
						segment[segmentSize*2] = xp ;
						segment[segmentSize*2+1] = yp ;
						segmentSize++;
					}
				}
			}

			/* change label to cleanedLabel if the segment is too small */
			if (segmentSize < minRegionSize) {
				while (segmentSize > 0) {
//					cleaned[segment[--segmentSize]] = cleanedLabel ;
					segmentSize-- ;
					vl_index seg_id = segment[segmentSize*2] + segment[segmentSize*2+1]*width ;
					cleaned[seg_id] = cleanedLabel ;
				}
			}
    	} // close x
	} // close y

#ifdef OMP
#pragma omp parallel for default (none) \
shared(segmentation,cleaned) \
firstprivate(numPixels) \
num_threads(NUM_THDS)
#endif
	/* restore base 0 indexing of the regions */
	for (pixel = 0 ; pixel < (signed)numPixels ; ++pixel){
		cleaned[pixel] -- ;
		segmentation[pixel] = cleaned[pixel];
	}

   vl_free(cleaned) ;
   vl_free(seg) ;

#ifdef LOG_SEGMENTATION_FINAL
	for(i=0; i<numPixels; i++)
		printf("%d\n", segmentation[i]) ;
#endif

#ifdef CHECKSUM_SEGMENTATION_FINAL
	int checksum_segmentation_final = 0 ;
	for(i=0; i<numPixels; i++)
		checksum_segmentation_final += segmentation[i] ;
	if(checksum_segmentation_final == CHECKSUM_SEGMENTATION_FINAL)
		printf("CHECKSUM FINAL SEGMENTATION:\tSUCCESS [%d vs %d]\n", checksum_segmentation_final, CHECKSUM_SEGMENTATION_FINAL) ;
	else
		printf("CHECKSUM FINAL SEGMENTATION:\tERROR [%d vs %d]\n", checksum_segmentation_final, CHECKSUM_SEGMENTATION_FINAL) ;
#endif

#endif
}


void vl_slic_segment(unsigned char * segmentation,
                     unsigned char * image,
                     vl_size width,
                     vl_size height,
                     vl_size numChannels,
                     vl_size regionSize,
                     vl_size regularization,
                     vl_size minRegionSize) {

	vl_index i, x, y, u, v, k, region ;
	vl_uindex iter ;

	vl_size const numRegionsX = (vl_size) fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(width), fixedpt_fromint(regionSize))));
	vl_size const numRegionsY = (vl_size) fixedpt_toint(fixedpt_ceil(fixedpt_div(fixedpt_fromint(height), fixedpt_fromint(regionSize))));
	vl_size const numRegions = numRegionsX * numRegionsY ;
	vl_size const numPixels = width * height ;
	fixedpt *centers ;
	int *edgeMap ;
	fixedpt previousEnergy = INT_MAX ;
	fixedpt startingEnergy ;
	fixedpt *masses ;
	vl_size const maxNumIterations = 100 ;

//	#define atimage(x,y,k) image[(x)+(y)*width+(k)*numPixels]
//	#define atEdgeMap(x,y) edgeMap[(x)+(y)*width]

	/* VL_MALLOC allocations */
	edgeMap = vl_calloc(numPixels, sizeof(int)) ;
	masses = vl_malloc(sizeof(fixedpt) * (2 + numChannels) * numRegions * NUM_THDS) ;
	centers = vl_malloc(sizeof(fixedpt) * (2 + numChannels) * numRegions * NUM_THDS) ;


/* ============================== STAGE 1: COMPUTE EDGE MAP (GRADIENT STRENGHT) ==============================*/

#ifdef PROFILING_LIB
	reset_timer();
	start_timer();
#endif

	/* compute edge map (gradient strength) */
	for(k = 0 ; k < (signed)numChannels ; ++k) {
		int k_0 = k*numPixels ;

#ifdef OMP
#pragma omp parallel for default (none) \
shared(image,edgeMap) \
private(x) \
firstprivate(height,width,numChannels,k_0) \
num_threads(NUM_THDS)
#endif
		for(y = 1 ; y < (signed)height-1 ; ++y) {

			int y_0 = y 	*  	width ;
			int y_1 = (y+1) * 	width ;
			int y_2 = (y-1) * 	width ;

			for(x = 1 ; x < (signed)width-1 ; ++x) {
				unsigned char a = image[(x-1)	+ y_0	+ k_0] ;
				unsigned char b = image[(x+1)	+ y_0	+ k_0] ;
				unsigned char c = image[x		+ y_1	+ k_0] ;
				unsigned char d = image[x		+ y_2	+ k_0] ;
				edgeMap[x+y_0] += (a - b) * (a - b) + (c - d) * (c - d) ;
			}
		}
	}

#ifdef PROFILING_LIB
	stop_timer();
	printf("STAGE 1:\t%d\n",get_time());
#endif

#ifdef LOG_GRADIENT
	for(i=0; i<1000; i++) // <numPixels
		printf("%d\n", edgeMap[i]) ;
#endif

#ifdef CHECKSUM_GRADIENT
  int checksum_gradient = 0 ;
  for(i=0; i<numPixels; i++)
	  checksum_gradient += edgeMap[i] ;
  if(checksum_gradient == CHECKSUM_GRADIENT)
	  printf("CHECKSUM GRADIENT:\t\tSUCCESS [%d vs %d]\n", checksum_gradient, CHECKSUM_GRADIENT) ;
  else
	  printf("CHECKSUM GRADIENT:\t\tERROR [%d vs %d]\n", checksum_gradient, CHECKSUM_GRADIENT) ;
#endif


/* =================================== STAGE 2: INITIALIZE K-MEANS CENTERS ===================================*/

#ifdef PROFILING_LIB
	reset_timer();
	start_timer();
#endif

#ifdef OMP
#pragma omp parallel for default (none) \
shared(edgeMap,image,centers) \
private(i,u,x,y,k) \
firstprivate(numRegionsY,numRegionsX,regionSize,width,height,numChannels) \
num_threads(NUM_THDS)
#endif
	for(v = 0; v < (signed)numRegionsY; ++v) {
		for(u = 0; u < (signed)numRegionsX; ++u) {
			vl_index xp ;
			vl_index yp ;
			int centerx = 0 ;
			int centery = 0 ;
			int minEdgeValue = INT_MAX ;

			x = fixedpt_toint(fixedpt_round(fixedpt_mul(fixedpt_fromint(regionSize), (fixedpt_fromint(u) + FIXEDPT_ONE_HALF))));
			y = fixedpt_toint(fixedpt_round(fixedpt_mul(fixedpt_fromint(regionSize), (fixedpt_fromint(v) + FIXEDPT_ONE_HALF))));
//if(u==0 && v==1) printf("%d %d\n",x,y); //3 3

			x = VL_MAX(VL_MIN(x, (signed)width-1),0) ;
			y = VL_MAX(VL_MIN(y, (signed)height-1),0) ;

			/* search in a 3x3 neighborhood the smallest edge response centered on the region center*/
			for(yp = VL_MAX(0, y-1) ; yp <= VL_MIN((signed)height-1, y+1) ; ++ yp) {
				for(xp = VL_MAX(0, x-1) ; xp <= VL_MIN((signed)width-1, x+1) ; ++ xp) {
//					if(u==1 && v==0) printf("%d %d\n",xp,yp);
					int thisEdgeValue = edgeMap[(xp)+(yp)*width];
					if (thisEdgeValue < minEdgeValue) {
						minEdgeValue = thisEdgeValue ;
						centerx = xp ;
						centery = yp ;
					}
				}
			}

			/* initialize the new center at this location */
			i = (u + v*numRegionsX)*3 ;
			centers[i++] = fixedpt_fromint(centerx) ;
			centers[i++] = fixedpt_fromint(centery) ;
//			printf("%d %d\n",centerx,centery) ;
			for (k  = 0 ; k < (signed)numChannels ; ++k) {
				centers[i++] = fixedpt_fromint((int)image[(centerx)+(centery)*width+(k)*numPixels]);
//				printf("%d\n",image[(centerx)+(centery)*width+(k)*numPixels]) ;
			}
		}
	}

#ifdef PROFILING_LIB
	stop_timer();
	printf("STAGE 2:\t%d\n",get_time());
#endif

#ifdef LOG_CENTERS
  for(i=0; i<((2 + numChannels) * numRegions); i++)
		fixedpt_print(centers[i]) ;
#endif

#ifdef CHECKSUM_CENTERS
  int checksum_centers = 0 ;
  for(i=0; i<((2 + numChannels) * numRegions); i++)
	  checksum_centers += fixedpt_toint(centers[i]) ;
  if(checksum_centers == CHECKSUM_CENTERS)
  	  printf("CHECKSUM CENTERS:\t\tSUCCESS [%d vs %d]\n", checksum_centers, CHECKSUM_CENTERS) ;
    else
  	  printf("CHECKSUM CENTERS:\t\tERROR [%d vs %d]\n", checksum_centers, CHECKSUM_CENTERS) ;
#endif




/* ===================================== STAGE 3: RUN K-MEANS ITERATIONS =====================================*/

#ifdef PROFILING_LIB
	reset_timer();
	start_timer();
#endif

  	fixedpt 	one_over_regionSize = fixedpt_div(FIXEDPT_ONE, fixedpt_fromint(regionSize)) ;
  	fixedpt		ten_thousand 		= fixedpt_frominttoHR(100000) ;
  	vl_index 	*u_array 			= vl_malloc((width+regionSize)*sizeof(vl_index)) ;
  	vl_index 	*v_array 			= vl_malloc((height+regionSize)*sizeof(vl_index)) ;
  	fixedpt 	factor 				= fixedpt_div(fixedpt_fromint(regularization), fixedpt_fromint(regionSize * regionSize)) ;
    int off 						= (2 + numChannels)*numRegions ;
  	fixedpt 	*masses_ptr ;
  	fixedpt 	*centers_ptr ;

//    int id ;
//    i = 0 ;
//
//    /* Init first ceil(regionSize/2) elements */
//    for(id=regionSize; id>0; id-=2) {
//    	v_array[i] = -1 ;
//		u_array[i] = -1 ;
//		i++ ;
//    }
//
//    id = 0 ;

#ifdef OMP
#pragma omp parallel for default(none) shared(v_array,u_array) private(x) firstprivate(height,width,one_over_regionSize) num_threads(NUM_THDS)
#endif
//    for (y = 0 ; y < (signed)height-i ; y+=regionSize) {
//    	for(k=0; k<regionSize; k++) {
//    		v_array[y+k+i] = id;
//    	}
//    	id++;
//    }

    for (y = 0 ; y < (signed)height ; y++)
    	v_array[y] = fixedpt_toint(fixedpt_floor(fixedpt_mul(fixedpt_fromint(y),one_over_regionSize) - FIXEDPT_ONE_HALF));

//    id = 0 ;

#ifdef OMP
        #pragma omp parallel for default(none) shared(v_array,u_array) private(y) firstprivate(height,width,one_over_regionSize) num_threads(NUM_THDS)
#endif
//    for (x = 0 ; x < (signed)width-i ; x+=regionSize) {
//    	for(k=0; k<regionSize; k++) {
//    		u_array[x+k+i] = id;
//    	}
//    	id++;
//    }

    for (x = 0 ; x < (signed)width ; x++)
    	u_array[x] = fixedpt_toint(fixedpt_floor(fixedpt_mul(fixedpt_fromint(x),one_over_regionSize) - FIXEDPT_ONE_HALF));

//    for (y = 0 ; y < (signed)height ; y++)
//      	printf("%d\n",v_array[y]);

//    for (x = 0 ; x < (signed)width ; x++)
//    	printf("%d\n",u_array[x]);

#ifdef LOG_KMEANS
#endif

#ifdef CHECKSUM_KMEANS
	int checksum_kmeans_u = 0 ;
	int checksum_kmeans_v = 0 ;

	for(i=0; i<height; i++)
		checksum_kmeans_v += v_array[i] ;
	for(i=0; i<width; i++)
		checksum_kmeans_u += u_array[i] ;

	int check = (checksum_kmeans_v * width)+ (checksum_kmeans_u * height);
	if(check == CHECKSUM_KMEANS)
	  	  printf("CHECKSUM K-MEANS:\t\tSUCCESS [%d vs %d]\n", check, CHECKSUM_KMEANS) ;
	    else
	  	  printf("CHECKSUM K-MEANS:\t\tERROR [%d vs %d]\n", check, CHECKSUM_KMEANS) ;
#endif

	for (iter = 0 ; iter < maxNumIterations ; ++iter) {

		fixedpt *energy_array = vl_calloc(NUM_THDS, sizeof(fixedpt)) ;
		fixedpt energy = 0 ;

#ifdef OMP
#pragma omp parallel for default (none) \
shared(u_array,v_array,centers,image,segmentation,energy_array) \
private(x,k) \
firstprivate(height,width,iter,factor,numChannels) \
num_threads(NUM_THDS)
#endif
		/* assign pixels to centers */
		for (y = 0 ; y < (signed)height ; ++y) {

			int thd_id =
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
			for (x = 0 ; x < (signed)width ; ++x) {

				vl_index up, vp ;
				vl_index pixel = x + y * width ;
				fixedpt minDistance = INT_MAX ;
				vl_index u = u_array[x] ;
				vl_index v = v_array[y] ;

//				if(iter==0 && y<12) printf("%d %d - %d %d\n",x,y,u,v);


				for (vp = VL_MAX(0, v) ; vp <= VL_MIN((signed)numRegionsY-1, v+1) ; ++vp) {
					for (up = VL_MAX(0, u) ; up <= VL_MIN((signed)numRegionsX-1, u+1) ; ++up) {
						vl_index region = up  + vp * numRegionsX ;

						fixedpt centerx = centers[(2 + numChannels) * region + 0] ;
						fixedpt centery = centers[(2 + numChannels) * region + 1] ;
//						if(iter==0 && pixel==10) printf("%d %d %d\n",centerx,centery,region) ;

						fixedpt spatial = fixedpt_mul((fixedpt_fromint(x) - centerx), (fixedpt_fromint(x) - centerx)) + fixedpt_mul((fixedpt_fromint(y) - centery), (fixedpt_fromint(y) - centery)) ;
						fixedpt appearance = 0 ;
						fixedpt distance ;
						for (k = 0 ; k < (signed)numChannels ; ++k) {
							fixedpt centerz = centers[(2 + numChannels) * region + k + 2]  ;
							fixedpt z = fixedpt_fromint((int)image[(x)+(y)*width+(k)*numPixels]);
							appearance += fixedpt_mul((z - centerz), (z - centerz)) ;
						}
						distance = appearance + fixedpt_mul(factor, spatial) ;

//						if(iter==0 && pixel<63*12) printf("%d %d\n",pixel,region) ;
//						if(iter==0 && pixel==10) printf("%d\n",distance) ;

						if (minDistance > distance) {
							minDistance = distance ;
							segmentation[pixel] = (unsigned char) region ;
						}
//						if(iter==0 && pixel>a && pixel<b)
//							printf("[%d %d] %d %d %d %d %d\n",pixel,region,vp,up,centerx,centery,distance) ;
					}
				}
				energy_array[thd_id] += fixedpt_to_HR(minDistance) ;
			}
		}

		for(i=0; i<NUM_THDS; i++)
			energy += energy_array[i];

		/* check energy termination conditions */
		if (iter == 0) {
			startingEnergy = energy ;
		} else {
			if ((previousEnergy - energy) < fixedpt_div_HR((startingEnergy - energy), ten_thousand)) {
				break ;
			}
		}
		previousEnergy = energy ;

	    /* recompute centers */
//	    memset(masses, 0, sizeof(fixedpt) * width * height) ;
//	    memset(centers, 0, sizeof(fixedpt) * (2 + numChannels) * numRegions) ;

//#pragma omp parallel for default (none) shared(masses) private(i) firstprivate(width,height) num_threads(NUM_THDS)
//	    for(i=0; i<width * height; i++)
//	    	masses[i] = 0 ;

#ifdef OMP
#pragma omp parallel for default (none) shared(centers,masses) firstprivate(numChannels,numRegions) num_threads(NUM_THDS)
#endif
		for(i=0; i<(2 + numChannels)*numRegions*NUM_THDS; i++) {
	    	centers[i] = 0 ;
	    	masses[i] = 0 ;
	    }

#ifdef OMP
#pragma omp parallel for default (none) \
shared(centers,image,masses,segmentation) \
private(x,k,masses_ptr,centers_ptr) \
firstprivate(height,width,numChannels,off) \
num_threads(NUM_THDS)
#endif
	    for (y = 0 ; y < (signed)height ; ++y) {

			masses_ptr = masses + off *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
			centers_ptr = centers + off *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif

	    	for (x = 0 ; x < (signed)width ; ++x) {
				vl_index pixel = x + y * width ;
				vl_index region = segmentation[pixel] ;
				masses_ptr[region] += FIXEDPT_ONE ;
//				if(region==1) printf("%d) %d %d\n",iter,x,y) ;
				centers_ptr[region * (2 + numChannels) + 0] += x ;
				centers_ptr[region * (2 + numChannels) + 1] += y ;
				for (k = 0 ; k < (signed)numChannels ; ++k) {
					centers_ptr[region * (2 + numChannels) + k + 2] += (int)image[(x)+(y)*width+(k)*numPixels] ;
				}
	    	}
	    }

#ifdef OMP
#pragma omp parallel for default (none) shared(centers,masses) firstprivate(numChannels,numRegions,off) num_threads(NUM_THDS)
#endif
	    for(i=0; i<(2 + numChannels)*numRegions; i++) {
	    	int j ;
	    	for(j=1; j<NUM_THDS; j++) {
		    	masses[i] += masses[off*j+i] ;
		    	centers[i] += centers[off*j+i] ;
	    	}
	    }

#ifdef OMP
#pragma omp parallel for default (none) \
shared(centers,masses) \
private(i) \
firstprivate(numRegions,numChannels) \
num_threads(NUM_THDS)
#endif
		for (region = 0 ; region < (signed)numRegions ; ++region) {
			/* 1-e8, the smallest number in fixedpt is 1 = 0.000244140625 */
			fixedpt mass = MAX(masses[region], 1) ;
			for (	i = (2 + numChannels) * region ;
					i < (signed)(2 + numChannels) * (region + 1) ;
					++i) {
				centers[i] = fixedpt_div(fixedpt_fromint(centers[i]), mass) ;
			}
		}
	} // close Iter loop


#ifdef LOG_SEGMENTATION
		for(y=0; y<5; y++)
		for(x=0; x<width; x++){
			printf("[%d %d] :: %d\n",x,y,segmentation[y*width+x]);
//			if(segmentation[y*width+x]<13) printf("[%d %d] :: %d\n",x,y,segmentation[y*width+x]);

		}
#endif




#ifdef PROFILING_LIB
	stop_timer();
	printf("STAGE 3:\t%d\n",get_time());
#endif

#ifdef LOG_SEGMENTATION
	for(i=0; i<numPixels; i++)
		printf("%d\n", segmentation[i]) ;
#endif

#ifdef CHECKSUM_SEGMENTATION
	int checksum_segmentation = 0 ;
	for(i=0; i<numPixels; i++)
		checksum_segmentation += segmentation[i] ;
	if(checksum_segmentation == CHECKSUM_SEGMENTATION)
		printf("CHECKSUM SEGMENTATION:\t\tSUCCESS [%d vs %d]\n", checksum_segmentation, CHECKSUM_SEGMENTATION) ;
	else
		printf("CHECKSUM SEGMENTATION:\t\tERROR [%d vs %d]\n", checksum_segmentation, CHECKSUM_SEGMENTATION) ;
#endif

	vl_free(masses) ;
	vl_free(centers) ;
	vl_free(edgeMap) ;




/* ==================================== STAGE 4: ELIMINATE SMALL REGIONS =====================================*/

#ifdef PROFILING_LIB
	reset_timer();
	start_timer();
#endif

	unsigned char 	*cleaned = vl_calloc(numPixels, sizeof(unsigned char)) ;
//	vl_uint16 		*segment = vl_malloc(sizeof(vl_uint16) * numPixels * 2) ;
	vl_uint16 		*seg = vl_malloc(sizeof(vl_uint16) * TEMP_SEG_BUFFER * 2 * NUM_THDS) ;
	vl_size 		segmentSize ;
	unsigned char	label ;
	unsigned char 	cleanedLabel ;
	vl_size 		numExpanded ;
	vl_index const 	dx [] = {+1, -1,  0,  0} ;
	vl_index const 	dy [] = { 0,  0, +1, -1} ;
	vl_index 		direction ;
	vl_index 		pixel ;


//#pragma omp parallel for default (none) \
//shared(segmentation,cleaned,seg) \
//private(x,y,label,numExpanded,segmentSize,pixel,cleanedLabel,direction) \
//firstprivate(height,width,minRegionSize) \
//num_threads(NUM_THDS)

    for (y = 0 ; y < (signed)height ; ++y) {
    	for (x = 0 ; x < (signed)width ; ++x) {

    		vl_index pixel = x + y * width ;

			if (cleaned[pixel]) continue ;

			label = segmentation[pixel] ;
			numExpanded = 0;
			segmentSize = 0;
			vl_uint16 *segment = seg + 100 *
#ifdef OMP
					omp_get_thread_num() ;
#else
					get_core_id() ;
#endif
//		    segment[segmentSize++] = pixel ;
			segment[segmentSize*2] = x ;
			segment[segmentSize*2+1] = y ;
			segmentSize++ ;

			/*
			 * find cleanedLabel as the label of an already cleaned
			 * region neighbor of this pixel
			 */

			cleanedLabel   = label + 1 ;
			cleaned[pixel] = label + 1 ;
//			x = pixel % width ;
//			y = pixel / width ;
			for (direction = 0 ; direction < 4 ; ++direction) {
				vl_index xp = x + dx[direction] ;
				vl_index yp = y + dy[direction] ;
				vl_index neighbor = xp + yp * width ;
				if (0 <= xp &&
					xp < (signed)width &&
					0 <= yp && yp < (signed)height &&
					cleaned[neighbor]) {
					cleanedLabel = cleaned[neighbor] ;
				}
			}

			/* expand the segment */
			while (numExpanded < segmentSize) {
//				vl_size open = segment[numExpanded++] ;
//				vl_index x_2 = open % width ;
//				vl_index y_2 = open / width ;
				vl_index x_2 = segment[numExpanded*2] ;
				vl_index y_2 = segment[numExpanded*2+1] ;
				numExpanded++ ;
				for (direction = 0 ; direction < 4 ; ++direction) {
					vl_index xp = x_2 + dx[direction] ;
					vl_index yp = y_2 + dy[direction] ;
					vl_index neighbor = xp + yp * width ;
					if (	0 <= xp && xp < (signed)width &&
							0 <= yp && yp < (signed)height &&
							cleaned[neighbor] == 0 &&
							segmentation[neighbor] == label) {
						cleaned[neighbor] = label + 1 ;
//						segment[segmentSize++] = neighbor ;
						segment[segmentSize*2] = xp ;
						segment[segmentSize*2+1] = yp ;
						segmentSize++;
					}
				}
			}

			/* change label to cleanedLabel if the segment is too small */
			if (segmentSize < minRegionSize) {
				while (segmentSize > 0) {
//					cleaned[segment[--segmentSize]] = cleanedLabel ;
					segmentSize-- ;
					vl_index seg_id = segment[segmentSize*2] + segment[segmentSize*2+1]*width ;
					cleaned[seg_id] = cleanedLabel ;
				}
			}
    	} // close x
	} // close y

#ifdef OMP
#pragma omp parallel for default (none) \
shared(segmentation,cleaned) \
firstprivate(numPixels) \
num_threads(NUM_THDS)
#endif
	/* restore base 0 indexing of the regions */
	for (pixel = 0 ; pixel < (signed)numPixels ; ++pixel){
		cleaned[pixel] -- ;
		segmentation[pixel] = cleaned[pixel];
	}

#ifdef PROFILING_LIB
	stop_timer();
	printf("STAGE 4:\t%d\n",get_time());
#endif

   vl_free(cleaned) ;
   vl_free(seg) ;

#ifdef LOG_SEGMENTATION_FINAL
	for(i=0; i<numPixels; i++)
		printf("%d\n", segmentation[i]) ;
#endif

#ifdef CHECKSUM_SEGMENTATION_FINAL
	int checksum_segmentation_final = 0 ;
	for(i=0; i<numPixels; i++)
		checksum_segmentation_final += segmentation[i] ;
	if(checksum_segmentation_final == CHECKSUM_SEGMENTATION_FINAL)
		printf("CHECKSUM FINAL SEGMENTATION:\tSUCCESS [%d vs %d]\n", checksum_segmentation_final, CHECKSUM_SEGMENTATION_FINAL) ;
	else
		printf("CHECKSUM FINAL SEGMENTATION:\tERROR [%d vs %d]\n", checksum_segmentation_final, CHECKSUM_SEGMENTATION_FINAL) ;
#endif

}
