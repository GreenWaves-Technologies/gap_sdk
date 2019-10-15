#include <stdio.h>
#include <math.h>
#include "hog.h"

/* ---------------------------------------------------------------- */
/** @brief Create a new HOG object
 ** @param variant HOG descriptor variant.
 ** @param numOrientations number of distinguished orientations.
 ** @param transposed wether images are transposed (column major).
 ** @return the new HOG object.
 **
 ** The function creates a new HOG object to extract descriptors of
 ** the prescribed @c variant. The angular resolution is set by
 ** @a numOrientations, which specifies the number of <em>undirected</em>
 ** orientations. The object can work with column major images
 ** by setting @a transposed to true.
 **/

static inline int getModulo(int entry, int mem_dim)
{
    while(entry >= mem_dim) entry -= mem_dim;
    return entry;
}

void vl_hog_prepare_buffers (VlHog * self, vl_size width, vl_size height, vl_size cellSize) {

    vl_size hogWidth = (width + cellSize/2) / cellSize ;
    vl_size hogHeight = (height + cellSize/2) / cellSize ;

    self->hog = vl_calloc(hogWidth * hogHeight * self->numOrientations * 2, sizeof(fixedpt)) ;
    //TODO: to be fixed. create at the full size cellW*cellH*fixed = 1KB
    self->hogNorm = vl_calloc(16 * 16, sizeof(fixedpt)) ;
    self->hogWidth = hogWidth ;
    self->hogHeight = hogHeight ;
}


VL_EXPORT VlHog * vl_hog_new (vl_size numOrientations, vl_bool transposed) {

    vl_index o, k ;
    VlHog * self = vl_calloc((size_t)1, (size_t)sizeof(VlHog)) ;

    self->numOrientations = numOrientations ;
    self->glyphSize = 21 ;
    self->transposed = transposed ;
    self->useBilinearOrientationAssigment = VL_FALSE ;
    self->orientationX = vl_malloc(sizeof(fixedpt) * self->numOrientations) ;
    self->orientationY = vl_malloc(sizeof(fixedpt) * self->numOrientations) ;

    /*
      Create a vector along the center of each orientation bin. These
      are used to map gradients to bins. If the image is transposed,
      then this can be adjusted here by swapping X and Y in these
      vectors.
    */
    fixedpt a = fixedpt_div(FIXEDPT_PI, fixedpt_fromint(self->numOrientations));
    for(o = 0 ; o < (signed)self->numOrientations ; ++o) {
        fixedpt angle = fixedpt_mul(fixedpt_fromint(o), a);

        if (!self->transposed) {
            self->orientationX[o] = fixedpt_cos(angle) ;
            self->orientationY[o] = fixedpt_sin(angle) ;
        }
        else {
            self->orientationX[o] = fixedpt_sin(angle) ;
            self->orientationY[o] = fixedpt_cos(angle) ;
        }
    }


    /*
      If the number of orientation is equal to 9, one gets:
      DalalTriggs:: 9 undirected orientations x 4 blocks.
    */
    self->dimension = 4*self->numOrientations;


    /*
      A permutation specifies how to permute elements in a HOG
      descriptor to flip it horizontally. Since the first orientation
      of index 0 points to the right, this must be swapped with orientation
      self->numOrientation that points to the left (for the directed case,
      and to itself for the undirected one).
    */

    self->permutation = vl_malloc(self->dimension * sizeof(vl_index)) ;

    for(k = 0 ; k < 4 ; ++k) {
        /* Find the corresponding block. Blocks are listed in order 1,2,3,4,...
           from left to right and top to bottom */
        vl_index blockx = k % 2 ;
        vl_index blocky = k / 2 ;
        vl_index q = (1 - blockx) + blocky * 2 ;
        for(o = 0 ; o < (signed)self->numOrientations ; ++o) {
            vl_index op = self->numOrientations - o ;
            self->permutation[o + k*self->numOrientations] = (op % self->numOrientations) + q*self->numOrientations ;
        }
    }


    /*
      Create glyphs for representing the HOG features/ filters. The glyphs
      are simple bars, oriented orthogonally to the gradients to represent
      image edges. If the object is configured to work on transposed image,
      the glyphs images are also stored in column-major.
    */

    // TODO: GLIPH required only in case of rendering final descriptor as an image
    //self->glyphs = vl_calloc(self->glyphSize * self->glyphSize * self->numOrientations, sizeof(fixedpt)); // 15 KB



// #define atglyph(x,y,k) self->glyphs[(x) + self->glyphSize * (y) + self->glyphSize * self->glyphSize * (k)]

//     fixedpt b = fixedpt_div(FIXEDPT_PI, fixedpt_fromint(self->numOrientations));

//     for (o = 0 ; o < (signed)self->numOrientations ; ++o) {

//         fixedpt angle = fixedpt_mod(fixedpt_mul(fixedpt_fromint(o), a) + FIXEDPT_HALF_PI, FIXEDPT_PI);

//         fixedpt angle_cos = fixedpt_cos(angle);
//         fixedpt angle_sin = fixedpt_sin(angle);

//         fixedpt x2 = fixedpt_mul(fixedpt_fromint(self->glyphSize), fixedpt_div(angle_cos, FIXEDPT_TWO));
//         fixedpt y2 = fixedpt_mul(fixedpt_fromint(self->glyphSize), fixedpt_div(angle_sin, FIXEDPT_TWO));

//         /* angle <= Pi/4 or angle >= 3/4Pi */
//         if(angle <= fixedpt_div(FIXEDPT_PI, fixedpt_fromint(4)) || angle >= fixedpt_mul(FIXEDPT_PI, fixedpt_div(fixedpt_fromint(3), fixedpt_fromint(4)))) {

//             /* along horizontal direction */
//             fixedpt slope = fixedpt_div(y2, x2);
//             fixedpt offset = fixedpt_div(fixedpt_mul((FIXEDPT_ONE - slope), (fixedpt_fromint(self->glyphSize) - FIXEDPT_ONE)), FIXEDPT_TWO) ;
//             /* (1-abs(cos(angle)))/2*glyphSize */
//             vl_index skip = fixedpt_toint(fixedpt_mul(fixedpt_div((FIXEDPT_ONE - fixedpt_abs(angle_cos)), FIXEDPT_TWO), fixedpt_fromint(self->glyphSize))) ;
//             vl_index i, j ;

//             for (i = skip ; i < (signed)self->glyphSize - skip ; ++i) {
//                 j = fixedpt_toint(fixedpt_round(fixedpt_mul(slope, fixedpt_fromint(i)) + offset));
//                 if (! self->transposed) {
//                     atglyph(i,j,o) = FIXEDPT_ONE_HP ;
//                 } else {
//                     atglyph(j,i,o) = FIXEDPT_ONE_HP ;
//                 }
//             }
//         } else {

//             /* along vertical direction */
//             fixedpt slope = fixedpt_div(x2, y2);
//             fixedpt offset = fixedpt_div(fixedpt_mul((FIXEDPT_ONE - slope), (fixedpt_fromint(self->glyphSize) - FIXEDPT_ONE)), FIXEDPT_TWO) ;
//             /* (1-sin(angle))/2*gliphSize */
//             vl_index skip = fixedpt_toint(fixedpt_mul(fixedpt_div((FIXEDPT_ONE - angle_sin), FIXEDPT_TWO), fixedpt_fromint(self->glyphSize))) ;
//             vl_index i, j ;

//             for (j = skip ; j < (signed)self->glyphSize - skip; ++j) {
//                 i = fixedpt_toint(fixedpt_round(fixedpt_mul(slope, fixedpt_fromint(j)) + offset));
//                 if (! self->transposed) {
//                     atglyph(i,j,o) = FIXEDPT_ONE_HP ;
//                 } else {
//                     atglyph(j,i,o) = FIXEDPT_ONE_HP ;
//                 }
//             }
//         }
//     }

    return self;
}


VL_EXPORT void vl_hog_put_image (VlHog *self, unsigned char *image, vl_size width, vl_size height, vl_size numChannels, vl_size cellSize, vl_size iteration) {

    vl_size stripe_elements_1st = VL_MAX(NUM_THDS*2, 3) ;
    vl_size nb_stripe_buffer = stripe_elements_1st + 1;
    vl_size nb_stripe_buffer_out = NUM_THDS;

    vl_index y_iter = cellSize ;
    vl_size hog_stripe_size, mem_dim, offset ;
    vl_size hogStride ;

    // useless with GS images (only one channel)
    vl_size channelStride = width * height ;
    vl_index x, y, i;
    vl_uindex k = 0 ;

#ifdef APPROX
    // one_over_cellSize may be computed here!
    fixedpt one_over_cellSize = fixedpt_div(FIXEDPT_ONE , fixedpt_fromint(cellSize));
    fixedpt *sqrt_coef_x0, *sqrt_coef_y0, *sqrt_coef_m;
    fixedpt_sqrt_init(&sqrt_coef_x0, &sqrt_coef_y0, &sqrt_coef_m);
#endif

    if(iteration == 0) {
        vl_hog_prepare_buffers(self, width, height+cellSize, cellSize) ; //ok
        y_iter = 0 ;
        offset = 0 ;
    }

    hog_stripe_size = self->hogWidth *self->numOrientations * 2 ; //ok
    mem_dim = hog_stripe_size * nb_stripe_buffer ;

    /* SET TO 0s THE RECEICLED BUFFER */
    if(iteration != 0) {
        offset = hog_stripe_size*(((stripe_elements_1st-1)+((iteration-1)*NUM_THDS))%nb_stripe_buffer) ;
#ifdef OMP
#pragma omp parallel num_threads(NUM_THDS)
#endif
        {
            memset(    self->hog+((((iteration-1)*nb_stripe_buffer_out*hog_stripe_size)+
#ifdef OMP
                                   omp_get_thread_num()
#else
                                   get_core_id()
#endif
                                   *hog_stripe_size)%mem_dim),
                       0,
                       hog_stripe_size*sizeof(fixedpt)) ; //ok
        }
    }

// x + y * 16 + k * 256 // x[0,127] , y[0,127], k[0-18]
//#define at(x,y,k) (self->hog[(x) + (y) * self->hogWidth + (k) * hogStride])
//#define at(x,y,k) (self->hog[(k) + (x) * self->numOrientations*2 + (y) * self->hogWidth*self->numOrientations*2])
//#define at(x,y,k) (self->hog[(((k) + (x) * self->numOrientations*2 + (y) * self->hogWidth*self->numOrientations*2)+offset)%mem_dim]) //ok

    int orts = (self->numOrientations) * 2;
    int wdth = self->hogWidth;
    int temp = orts * wdth;

    /* compute gradients and map the to HOG cells by bilinear interpolation */
//    #pragma omp parallel for num_threads(NUM_THDS)
#ifdef OMP
#ifdef APPROX
#pragma omp parallel for default (none) shared(self, image, sqrt_coef_x0, sqrt_coef_y0, sqrt_coef_m) private(x) \
    firstprivate(height,width,k,channelStride,numChannels,one_over_cellSize,cellSize,y_iter,mem_dim,offset,iteration,orts,temp) \
    num_threads(NUM_THDS) schedule(dynamic)
#else
#pragma omp parallel for default (none) shared(self, image) private(x)  \
    firstprivate(height,width,k,channelStride,numChannels,cellSize,y_iter,mem_dim,offset,iteration,orts,temp) \
    num_threads(NUM_THDS) schedule(dynamic)
#endif
#endif

    for (y = 1 ; y < (signed)height - 1 ; ++y) {
        for (x = 1 ; x < (signed)width - 1 ; ++x) {

            fixedpt gradx = 0 ;
            fixedpt grady = 0 ;
            fixedpt grad ;
            fixedpt orientationWeights [2] = {0,0} ;
            vl_index orientationBins [2] = {-1,-1} ;
            vl_index orientation = 0 ;
            fixedpt hx, hy, wx1, wx2, wy1, wy2 ;
            vl_index binx, biny, o ;

            /*
              Compute the gradient at (x,y). The image channel with
              the maximum gradient at each location is selected.
            */

            /* TODO: if we use only Gray-Scale img, we could avoid
               the LOOP and the IF statment inside the loop */
            unsigned char const * iter = image + y * width + x ;
            int grad2 = 0 ;

            for (k = 0 ; k < numChannels ; ++k) {
                int gradx_ = (int)(*(iter + 1) - *(iter - 1)) ;
                int grady_ = (int)(*(iter + width)  - *(iter - width)) ;
                int grad2_ = (gradx_ * gradx_) + (grady_ * grady_) ;
                if (grad2_ > grad2) {
                    gradx = fixedpt_fromint(gradx_) ;
                    grady = fixedpt_fromint(grady_) ;
                    grad2 = grad2_ ;
                }
                iter += channelStride ;
            }

//            if(x==1&&y==1) {
//                printf("%d\n",grad2);
//                fixedpt axd = fixedpt_fromint(grad2);
//TODO lo devi trattare come un 16 bit salvato in un 32 bit e non come un 32 bit
//                fixedpt_print_special32(axd);
//            }
#ifndef APPROX
            grad = fixedpt_sqrt(fixedpt_fromint(grad2)) ;
#else
            grad = fixedpt_sqrt_approx(fixedpt_fromint(grad2), sqrt_coef_x0, sqrt_coef_y0, sqrt_coef_m) ;
#endif
//            if(x==1&&y==1)fixedpt_print(grad);
            /* NOTE: 1 in fixedpt is the smallest number possible */
#ifdef APPROX
            fixedpt one_over_max = fixedpt_div(FIXEDPT_ONE , MAX(grad, 1));
            gradx = fixedpt_mul(gradx, one_over_max) ;
            grady = fixedpt_mul(grady, one_over_max) ;
#else
            fixedpt max = MAX(grad, 1);
            gradx = fixedpt_div(gradx, max) ;
            grady = fixedpt_div(grady, max) ;
#endif


            /*
              Map the gradient to the closest and second closets orientation bins.
              There are numOrientations orientation in the interval [0,pi).
              The next numOriantations are the symmetric ones, for a total
              of 2*numOrientation directed orientations.
            */
            for (k = 0 ; k < self->numOrientations ; ++k) {
#ifndef APPROX
                fixedpt orientationScore_ = fixedpt_mul(gradx, self->orientationX[k]) + fixedpt_mul(grady, self->orientationY[k]) ;
#else
                fixedpt orientationScore_ = (gradx*self->orientationX[k] + grady*self->orientationY[k]) >> FIXEDPT_FBITS;
#endif
                vl_index orientationBin_ = k ;

                if (orientationScore_ < 0) {
                    orientationScore_ = - orientationScore_ ;
                    orientationBin_ += self->numOrientations ;
                }

//                if(x==25&&y==4) {
//                        fixedpt_print(gradx);
//                        fixedpt_print(grady);
//                        fixedpt_print( self->orientationX[k]);
//                        }

                if (orientationScore_ > orientationWeights[0]) {
                    orientationBins[1] = orientationBins[0] ;
                    orientationWeights[1] = orientationWeights[0] ;
                    orientationBins[0] = orientationBin_ ;
                    orientationWeights[0] = orientationScore_ ;
                } else if (orientationScore_ > orientationWeights[1]) {
                    orientationBins[1] = orientationBin_ ;
                    orientationWeights[1] = orientationScore_ ;
                }
//                if(x==25&&y==4)
//                    printf("%d %d\n",orientationBins[0],orientationBins[1]);
//        //                    fixedpt_print(tmp11);
//        //                    fixedpt_print(tmp12);
//        //                    fixedpt_print(tmp21);
//        //                    fixedpt_print(tmp22);
//                        }
            }

            if (self->useBilinearOrientationAssigment) {
                /* min(1.0,...) guards against small overflows causing NaNs */
//                float angle0 = acosf(VL_MIN(orientationWeights[0],1.0)) ;
//                orientationWeights[1] = angle0 / (VL_PI / self->numOrientations) ;
//                orientationWeights[0] = 1 - orientationWeights[1] ;
//                _printf("FATAL ERROR: Using not fixedpoint procedure!\n");
                printf("ERROR useBilinearOrientationAssigment not implemented.\n");
            } else {
                orientationWeights[0] = FIXEDPT_ONE ;
                orientationBins[1] = -1 ;
            }

//            if(x==25&&y==4)
//                printf("%d %d\n",orientationBins[0],orientationBins[1]);

            for (o = 0 ; o < 2 ; ++o) {

                /*
                  Accumulate the gradient. hx is the distance of the
                  pixel x to the cell center at its left, in units of cellSize.
                  With this parametrixation, a pixel on the cell center
                  has hx = 0, which gradually increases to 1 moving to the next
                  center.
                */

                orientation = orientationBins[o] ;
                if (orientation < 0) continue ;

                /*  (x - (w-1)/2) / w = (x + 0.5)/w - 0.5 */
#ifdef APPROX
                hx = fixedpt_mul((fixedpt_fromint(x) + FIXEDPT_ONE_HALF), one_over_cellSize) - FIXEDPT_ONE_HALF ;
                hy = fixedpt_mul((fixedpt_fromint(y+y_iter) + FIXEDPT_ONE_HALF), one_over_cellSize) - FIXEDPT_ONE_HALF ;
#else
                hx = fixedpt_div((fixedpt_fromint(x) + FIXEDPT_ONE_HALF), fixedpt_fromint(cellSize)) - FIXEDPT_ONE_HALF ;
                //hy = fixedpt_div((fixedpt_fromint(y) + fixedpt_rconst(0.5)), fixedpt_fromint(cellSize)) - fixedpt_rconst(0.5) ;
                hy = fixedpt_div((fixedpt_fromint(y+y_iter) + FIXEDPT_ONE_HALF), fixedpt_fromint(cellSize)) - FIXEDPT_ONE_HALF ;
#endif
//#else
//                hx = fixedpt_div_HR((fixedpt_frominttoHR(x) + fixedpt_rconst_HR(0.5)), fixedpt_frominttoHR(cellSize)) - fixedpt_rconst_HR(0.5) ;
//                //hy = fixedpt_div((fixedpt_fromint(y) + fixedpt_rconst(0.5)), fixedpt_fromint(cellSize)) - fixedpt_rconst(0.5) ;
//                hy = fixedpt_div_HR((fixedpt_frominttoHR(y+y_iter) + fixedpt_rconst_HR(0.5)), fixedpt_frominttoHR(cellSize)) - fixedpt_rconst_HR(0.5) ;
//#endif


                binx = fixedpt_toint(fixedpt_floor(hx));
                biny = fixedpt_toint(fixedpt_floor(hy));
//                binx = fixedpt_toint_HR(fixedpt_floor_HR(hx));
//                biny = fixedpt_toint_HR(fixedpt_floor_HR(hy));

                wx2 = hx - fixedpt_fromint(binx) ;
                wy2 = hy - fixedpt_fromint(biny) ;
//                wx2 = (hx & 0x00000007) << (FIXEDPT_FBITS-FIXEDPT_FBITS_HR);
//                wy2 = (hy & 0x00000007) << (FIXEDPT_FBITS-FIXEDPT_FBITS_HR);

                wx1 = FIXEDPT_ONE - wx2 ;
                wy1 = FIXEDPT_ONE - wy2 ;

                wx1 = fixedpt_mul(wx1, orientationWeights[o]) ;
                wx2 = fixedpt_mul(wx2, orientationWeights[o]) ;
                wy1 = fixedpt_mul(wy1, orientationWeights[o]) ;
                wy2 = fixedpt_mul(wy2, orientationWeights[o]) ;


                fixedpt tmp11 = fixedpt_mul(fixedpt_mul(grad, wx1), wy1);
                fixedpt tmp12 = fixedpt_mul(fixedpt_mul(grad, wx1), wy2);
                fixedpt tmp21 = fixedpt_mul(fixedpt_mul(grad, wx2), wy1);
                fixedpt tmp22 = fixedpt_mul(fixedpt_mul(grad, wx2), wy2);
//                if(x==25&&y==4) {
//                    printf("%d %d %d\n",binx,biny,orientation);
////                    fixedpt_print(tmp11);
////                    fixedpt_print(tmp12);
////                    fixedpt_print(tmp21);
////                    fixedpt_print(tmp22);
//                }

                if (binx >= 0 && biny >=0) {
                    int entry = orientation + (binx * orts) + (biny * temp);
                    entry = getModulo((entry + offset), mem_dim);
//                    if(x==1)
//                        printf("%d\n",entry);
#ifdef OMP
#pragma omp critical
#endif
                    {
                        self->hog[entry] += tmp11 ;
                    }
                }
                if (binx < (signed)self->hogWidth - 1 && biny >=0) {
                    int entry = orientation + ((binx+1) * orts) + (biny * temp);
                    entry = getModulo((entry + offset), mem_dim);
//                    if(x==1)
//                        printf("%d\n",entry);
#ifdef OMP
#pragma omp critical
#endif
                    {
                        self->hog[entry] += tmp21 ;
                    }
                }
                if (binx < (signed)self->hogWidth - 1 && biny < (signed)self->hogHeight - 1) {
                    int entry = orientation + ((binx+1) * orts) + ((biny+1) * temp);
                    entry = getModulo((entry + offset), mem_dim);
//                    if(x==1)
//                        printf("%d\n",entry);
#ifdef OMP
#pragma omp critical
#endif
                    {
                        self->hog[entry] += tmp22 ;
                    }
                }
                if (binx >= 0 && biny < (signed)self->hogHeight - 1) {
                    int entry = orientation + (binx * orts) + ((biny+1) * temp);
                    entry = getModulo((entry + offset), mem_dim);
//                    if(x==1)
//                        printf("%d\n",entry);
#ifdef OMP
#pragma omp critical
#endif
                    {
                        self->hog[entry] += tmp12 ;
                    }
                }
            } /* next o */
        } /* next x */
    } /* next y */
#ifdef APPROX
    fixedpt_sqrt_deinit(sqrt_coef_x0, sqrt_coef_y0, sqrt_coef_m);
#endif
}


VL_EXPORT void vl_hog_extract (VlHog * self, fixedpt * features, vl_index iteration) {

    vl_size stripe_elements_1st = VL_MAX(NUM_THDS*2, 3) ;
    vl_size nb_stripe_buffer = stripe_elements_1st + 1;
    vl_index total_hogHeight = 16;
    vl_size nb_stripe_buffer_out = VL_MIN(NUM_THDS, total_hogHeight-(iteration*NUM_THDS)) ;

    vl_index x, y, i = nb_stripe_buffer-NUM_THDS-2 ;
    vl_uindex k, j ;
    vl_uindex norm_iteration_stride = 0;
    vl_index remaning_tripes = (total_hogHeight-(stripe_elements_1st-1)-(iteration-1)*NUM_THDS) ;
    vl_index input_hogHeight = VL_MIN(NUM_THDS, remaning_tripes) ;
    vl_index offset = 0 ;

    vl_uindex hog_stripe_size = self->hogWidth * self->numOrientations * 2 ;
    vl_index hog_buffer_size = hog_stripe_size * nb_stripe_buffer;
    vl_index buffer_offset = hog_stripe_size*((iteration*NUM_THDS) % nb_stripe_buffer);

    /* First Iteration */
    if(iteration == 0) {
        i = 0 ;
        remaning_tripes = total_hogHeight ;
        input_hogHeight = stripe_elements_1st-1 ;
    }
    /* Intermediate Iterations */
    else {
        offset = hog_stripe_size * ((NUM_THDS*iteration+i) % nb_stripe_buffer) ;
        norm_iteration_stride = ((stripe_elements_1st-1) + (iteration-1) * NUM_THDS) * self->hogWidth ;
    }

//#define at(x,y,k) (self->hog[(x) + (y) * self->hogWidth + (k) * hogStride])
//#define atNorm(x,y) (self->hogNorm[(x) + (y) * self->hogWidth])
#define atNorm(x,y) (self->hogNorm[(x) + (y) * self->hogWidth])

    /*
      Computes the squared L2 norm of each HOG cell. This is the norm of the
      undirected orientation histogram, counting only numOrientations. This
      histogram is obtained by folding the 2*numOrientations directed
      orientations that are computed.
    */

    if(remaning_tripes > 0) {

        fixedpt * niter = self->hogNorm + norm_iteration_stride ;
#ifdef OMP
#pragma omp parallel for default (none) shared(self,niter,iteration) private(j) \
    firstprivate(input_hogHeight,hog_buffer_size,offset)                \
    num_threads(NUM_THDS)
#endif
        for (k = 0 ; k < self->hogWidth * input_hogHeight ; k++) {
            for (j = 0 ; j < self->numOrientations ; j++) {
                fixedpt h1 = self->hog[(offset + k*self->numOrientations*2 + j)%hog_buffer_size] ;
                fixedpt h2 = self->hog[(offset + k*self->numOrientations*2 + j + self->numOrientations)%hog_buffer_size] ;
                fixedpt h = h1 + h2 ;
                niter[k] += fixedpt_mul_HR(fixedpt_to_HR(h), fixedpt_to_HR(h)) ;
            }
        }
    }

    /*
      HOG block-normalisation. For each cell, there are four 2x2 blocks
      covering it. For example, the cell number 5 in the following scheme
      is covered by the four blocks 1245, 2356, 4578, 5689.

      +---+---+---+
      | 1 | 2 | 3 |
      +---+---+---+
      | 4 | 5 | 6 |
      +---+---+---+
      | 7 | 8 | 9 |
      +---+---+---+

      In the Dalal-Triggs implementation, one forms all possible 2x2 blocks
      of cells, computes a descriptor vector for each by stacking the corresponding
      2x2 HOG cells, and L2 normalizes (and truncates) the result.

      Thus each HOG cell appears in four blocks. These are then decomposed
      again to produce descriptors for each cell. Each descriptor is simply
      the stacking of the portion of each block descriptor that arised
      from that cell. This process result in a descriptor
      of each cell which contains four copies of the original HOG,
      with four different normalization factors.

      @remark By stacking together the cell descriptors for a large retangular block
      of cells, one effectively stacks together the block descriptors of
      an equal number of blocks (except for the boundaries, for which
      blocks are only partially included). Since blocks are L2 normalized
      (up to truncation), this implies that the L2 norm of the resulting
      vector is approximately equal to the area of the region.
    */

    vl_index iter_y_off = iteration*NUM_THDS ;
//    #pragma omp parallel for num_threads(NUM_THDS)
#ifdef OMP
#pragma omp parallel for default (none) shared(self,features) private(x,k) \
    firstprivate(iter_y_off,nb_stripe_buffer_out,total_hogHeight,buffer_offset,hog_buffer_size,hog_stripe_size,iteration) \
    num_threads(NUM_THDS)
#endif
    for (y = 0 ; (unsigned)y < nb_stripe_buffer_out ; ++y) {

        for (x = 0 ; x < (signed)self->hogWidth ; ++x) {

            /* norm of upper-left, upper-right, ... blocks */
            vl_index xm = MAX(x - 1, 0) ;
            vl_index xp = MIN(x + 1, (signed)self->hogWidth - 1) ;
            vl_index ym = MAX((y+iter_y_off) - 1, 0) ;
            vl_index yp = MIN((y+iter_y_off) + 1, (signed)total_hogHeight - 1) ;

            fixedpt norm1 = atNorm(xm,ym) ;
            fixedpt norm2 = atNorm(x,ym) ;
            fixedpt norm3 = atNorm(xp,ym) ;
            fixedpt norm4 = atNorm(xm,(y+iter_y_off)) ;
            fixedpt norm5 = atNorm(x,(y+iter_y_off)) ;
            fixedpt norm6 = atNorm(xp,(y+iter_y_off)) ;
            fixedpt norm7 = atNorm(xm,yp) ;
            fixedpt norm8 = atNorm(x,yp) ;
            fixedpt norm9 = atNorm(xp,yp) ;

            fixedpt factor1, factor2, factor3, factor4 ;

            fixedpt t1 = 0 ;
            fixedpt t2 = 0 ;
            fixedpt t3 = 0 ;
            fixedpt t4 = 0 ;

            fixedpt * oiter = features + (x*self->dimension + y*self->dimension*self->hogWidth) ;

            /* each factor is the inverse of the l2 norm of one of the 2x2 blocks surrounding cell x,y */

            /* if the image is transposed, y and x are swapped */
            if (self->transposed) {
                factor1 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm1 + norm2 + norm4 + norm5 + 1)))) ;
                factor3 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm2 + norm3 + norm5 + norm6 + 1)))) ;
                factor2 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm4 + norm5 + norm7 + norm8 + 1)))) ;
                factor4 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm5 + norm6 + norm8 + norm9 + 1)))) ;
            } else {
                factor1 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm1 + norm2 + norm4 + norm5 + 1)))) ;
                factor2 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm2 + norm3 + norm5 + norm6 + 1)))) ;
                factor3 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm4 + norm5 + norm7 + norm8 + 1)))) ;
                factor4 = fixedpt_div_HP(FIXEDPT_ONE_HP, fixedpt_to_HP(fixedpt_from_HR(fixedpt_sqrt_HR(norm5 + norm6 + norm8 + norm9 + 1)))) ;
            }

            for (k = 0 ; k < self->numOrientations ; ++k) {

                fixedpt ha = fixedpt_to_HP(self->hog[(buffer_offset + k + x*self->numOrientations*2 + y*hog_stripe_size)%hog_buffer_size]) ;
                fixedpt hb = fixedpt_to_HP(self->hog[(buffer_offset + k + x*self->numOrientations*2 + y*hog_stripe_size + self->numOrientations)%hog_buffer_size]) ;
                fixedpt hc ;

                fixedpt ha1 = fixedpt_mul_HP(factor1, ha) ;
                fixedpt ha2 = fixedpt_mul_HP(factor2, ha) ;
                fixedpt ha3 = fixedpt_mul_HP(factor3, ha) ;
                fixedpt ha4 = fixedpt_mul_HP(factor4, ha) ;

                fixedpt hb1 = fixedpt_mul_HP(factor1, hb) ;
                fixedpt hb2 = fixedpt_mul_HP(factor2, hb) ;
                fixedpt hb3 = fixedpt_mul_HP(factor3, hb) ;
                fixedpt hb4 = fixedpt_mul_HP(factor4, hb) ;

                fixedpt hc1 = ha1 + hb1 ;
                fixedpt hc2 = ha2 + hb2 ;
                fixedpt hc3 = ha3 + hb3 ;
                fixedpt hc4 = ha4 + hb4 ;

                fixedpt zptwo = fixedpt_rconst_HP(0.2);

                ha1 = MIN(zptwo, ha1) ;
                ha2 = MIN(zptwo, ha2) ;
                ha3 = MIN(zptwo, ha3) ;
                ha4 = MIN(zptwo, ha4) ;

                hb1 = MIN(zptwo, hb1) ;
                hb2 = MIN(zptwo, hb2) ;
                hb3 = MIN(zptwo, hb3) ;
                hb4 = MIN(zptwo, hb4) ;

                hc1 = MIN(zptwo, hc1) ;
                hc2 = MIN(zptwo, hc2) ;
                hc3 = MIN(zptwo, hc3) ;
                hc4 = MIN(zptwo, hc4) ;

                t1 += hc1 ;
                t2 += hc2 ;
                t3 += hc3 ;
                t4 += hc4 ;

                /* Overlapping # 1 */
                oiter[k] = hc1;
                /* Overlapping # 2 */
                oiter[k + self->numOrientations] = hc2;
                /* Overlapping # 3 */
                oiter[k + 2 * self->numOrientations] = hc3;
                /* Overlapping # 4 */
                oiter[k + 3 * self->numOrientations] = hc4;

            } /* next orientation */
        } /* next x */
    } /* next y */
}


VL_EXPORT void vl_hog_delete (VlHog * self) {

    if (self->orientationX) {
        vl_free(self->orientationX) ;
        self->orientationX = NULL ;
    }

    if (self->orientationY) {
        vl_free(self->orientationY) ;
        self->orientationY = NULL ;
    }

    if (self->glyphs) {
        vl_free(self->glyphs) ;
        self->glyphs = NULL ;
    }

    if (self->permutation) {
        vl_free(self->permutation) ;
        self->permutation = NULL ;
    }

    if (self->hog) {
        vl_free(self->hog) ;
        self->hog = NULL ;
    }

    if (self->hogNorm) {
        vl_free(self->hogNorm) ;
        self->hogNorm = NULL ;
    }

    vl_free(self) ;
}


VL_EXPORT vl_size vl_hog_get_height (VlHog * self) {

    return self->hogHeight;
}


VL_EXPORT vl_size vl_hog_get_width (VlHog * self) {

    return self->hogWidth;
}


VL_EXPORT vl_size vl_hog_get_dimension (VlHog const * self) {

    return self->dimension;
}


VL_EXPORT void hog_accuracy_eval(float *ref, fixedpt *test, int blocksOverlaps, int numOrientations, int hogWidth, int hogHeight, int verbose) {

    printf("--- HOG Accuracy Evaluation Routine ---\n");

    if (blocksOverlaps != 4)
        printf("Attention! Blocks Overlapping of %d. In the DalalTriggs algorithm this value should be 4!\n", blocksOverlaps) ;

    float max_ref ;
    fixedpt max_test ;
    int
        /* Old index format */
        dominant_ref_old = 0, dominant_test_old = 0 ,
        /* New index format */
        dominant_ref_new = 0, dominant_test_new = 0;
    int k_ref = 0, k_test = 0 ;
    int dominant_error = 0 ;
    float error_sum = 0.0f ;

    int x,y,z,j = 0 ;
    int X = blocksOverlaps ;        // 4
    int Y = numOrientations ;        // 9
    int Z = hogWidth*hogHeight ;    // 256

    for (z=0; z<Z; z++) {
        for (x=0; x<X; x++) {

            max_ref = 0.0f ;
            max_test = 0 ;
            dominant_ref_new = -1 ;
            dominant_test_new = -1 ;

            for (y=0; y<Y; y++, j++) {

                int i = x*Y*Z + y*Z + z ;

                if (ref[j] > max_ref) {
                    max_ref = ref[j] ;
                    dominant_ref_new = j ; // index of the dominant bin
                    dominant_ref_old = i;
                    k_ref = y ;
                }

                if (test[j] > max_test) {
                    max_test = test[j] ;
                    dominant_test_new = j ; // index of the dominant bin
                    dominant_test_old = i;
                    k_test = y ;
                }

            }

            if(dominant_ref_new != dominant_test_new) {
                if(verbose == 1) {
                    printf("ref %d [%d] test %d [%d]\n", dominant_ref_new, dominant_ref_old, dominant_test_new, dominant_test_old);
//                    printf("DOMINANCE ERROR: %d.%d [%d %d]\n", x+1, z+1, k_ref, k_test) ;
                }
                dominant_error ++ ;
                if(verbose == 1) printf("0.00\n") ;
                continue ;
            }

            float tmp = fixedpt_HP_to_float(test[dominant_ref_new]);
            float error = 0.0f;
            if(ref[dominant_ref_new] != 0.0f)
                error = fabs(100.0-((tmp*100.0)/ref[dominant_ref_new]));
            error_sum += error ;
            if(verbose == 1) {
                printf("%d.%d [%d %d] %f %f %.2f\n", x+1, z+1, k_ref, k_test, ref[dominant_ref_new], tmp, error);
                printf("%.2f\n", error);
            }
        }
    }

    printf("Bin Dominance Error: %d over %d Histograms\n", dominant_error, X*Z) ;
//    printf("Bin Dominance AVG Error: %.2f\%\n", (dominant_error*100.0f)/(X*Z)) ;
//    printf("Overall Bin's AVG Error: %.2f\%\n", error_sum/(X*Z-dominant_error)) ;

}
