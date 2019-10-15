#include <stdint.h>
#include "omp.h"
#include "common.h"

#define KERNEL_SIZE 3
#define NUM_CORES   8

//#define APP_DEBUG
#define PROFILING
#define CHECK

#define IMG_WIDTH  24
#define IMG_HEIGHT 20

#ifndef NO_SHORT_PIXEL
//#define SHORT_PIXEL
#endif

#ifndef NO_USE_INTRINSICS
#define USE_INTRINSICS
#endif

#ifndef NO_USE_OMP
#define USE_OMP
#endif

#define USE_TILING

#ifdef USE_TILING
#define PRECISE_TILING
#define TILE_WIDTH    12
#define TILE_HEIGHT   10
#define W_TILES       (IMG_WIDTH/TILE_WIDTH)
#define H_TILES       (IMG_HEIGHT/TILE_HEIGHT)
#define N_TILES       (W_TILES*H_TILES)
#define BORDER_SIZE   (KERNEL_SIZE/2)*4 // 4 = NxN filters in the pipeline
#define BUFFER_WIDTH  (TILE_WIDTH  + BORDER_SIZE)
#define BUFFER_HEIGHT (TILE_HEIGHT + BORDER_SIZE)
#else
#define BUFFER_WIDTH  IMG_WIDTH
#define BUFFER_HEIGHT IMG_HEIGHT
#endif

#ifdef SHORT_PIXEL
  typedef short pixel;
  typedef short coeff;
  typedef short  vector __attribute__ ((vector_size (4)));
  typedef short  coeff_vector __attribute__ ((vector_size (4)));
  inline vector vector_abs(vector x) { return __ABS2(x); }
  inline vector vector_max(vector x, vector y) { return __MAX2(x, y); }
  inline short max_element(vector x) { return x[0] > x[1]? x[0]: x[1]; }
  inline vector vector_min(vector x, vector y) { return __MIN2(x, y); }
  inline short min_element(vector x) { return x[0] < x[1]? x[0]: x[1]; }
  #define INIT_VECTOR(x) {x,x}
#else
  typedef unsigned char pixel;
  typedef char coeff;
  typedef unsigned char  vector __attribute__ ((vector_size (4)));
  typedef char  coeff_vector __attribute__ ((vector_size (4)));
  typedef signed char  signed_vector __attribute__ ((vector_size (4)));
  inline coeff_vector vector_abs(coeff_vector x) { return __ABS4(x); }
  inline coeff_vector vector_max(coeff_vector x, coeff_vector y) { return __MAX4(x, y); }
  inline unsigned char max_element(vector x) {
    unsigned char a = (x[0] > x[1])? x[0]: x[1];
    unsigned char b = (x[2] > x[3])? x[2]: x[3];
    return (a>b)? a: b;
  }
  inline coeff_vector vector_min(coeff_vector x, coeff_vector y) { return __MIN4(x, y); }
  inline unsigned char min_element(vector x) {
    unsigned char a = (x[0] < x[1])? x[0]: x[1];
    unsigned char b = (x[2] < x[3])? x[2]: x[3];
    return (a<b)? a: b;
  }
  #define INIT_VECTOR(x) {x,x,x,x}
#endif

#ifdef USE_INTRINSICS
#define VECTOR_SIZE (sizeof(vector)/sizeof(pixel))
#endif

//#include "data_5_5.h"
#include "data_20_24.h"

static void sum_image(pixel *restrict image_in1, pixel *restrict image_in2, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
  vector *image_in1_vec = (vector *) image_in1;
  vector *image_in2_vec = (vector *) image_in2;
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < W; j++)
      image_in1_vec[i * W + j] += image_in2_vec[i * W + j];
#else
  for (unsigned i = 0; i < height; i++) {
    for (unsigned j = 0; j < width; j++) {
      image_in1[i * width + j] =
            (image_in1[i * width + j])+(image_in2[i * width + j]);
    }
  }

#endif
}

static void sub_image(pixel *restrict image_in1, pixel *restrict image_in2,
                      pixel *restrict image_out,
                      unsigned width, unsigned height,
                      unsigned in2_stride, unsigned in2_offset) {
#ifdef USE_INTRINSICS
  vector *image_in1_vec = (vector *) image_in1;
  vector *image_in2_vec = (vector *) (image_in2+in2_offset);
  vector *image_out_vec = (vector *) image_out;
  const unsigned W  = width/VECTOR_SIZE;
  const unsigned W2 = in2_stride/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++) {
    for (unsigned j = 0; j < W; j++) {
      vector v = image_in2_vec[i * W2 + j];
      //if(j == W-1 && width%4 != 0) {
      //  v[2] = image_in2[(i+1) * in2_stride + in2_offset];
      //  v[3] = image_in2[(i+1) * in2_stride + in2_offset + 1];
      //    }
     image_out_vec[i * W + j] = vector_abs(image_in1_vec[i * W + j] -
                                           v);
    }
  }
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++) {
    for (unsigned j = 0; j < width; j++) {
      int val = (int)image_in1[i * width + j] - (int)image_in2[i * in2_stride + in2_offset + j];
      image_out[i * width + j] = (val>0? (short)val: (short)-val);
    }
  }
#endif
}

static pixel max_image(pixel *restrict image_in, unsigned width, unsigned height) {
  pixel max_pix = 0;

#ifdef USE_INTRINSICS
  vector *image_in_vec = (vector *) image_in;
  vector max_vector = INIT_VECTOR(0);
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < W; j++)
      max_vector = vector_max(image_in_vec[i * W + j], max_vector);
  max_pix = max_element(max_vector) ;
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < width; j++)
      if (image_in[i * width + j] > max_pix)
        max_pix = image_in[i * width + j];
#endif

  return max_pix;
}


static pixel max_sub_image(pixel *restrict image_in1, pixel *restrict image_in2,
                           unsigned width, unsigned height,
                           unsigned in2_stride, unsigned in2_offset) {
  pixel max_pix = 0;

#ifdef USE_INTRINSICS
  vector *image_in1_vec = (vector *) image_in1;
  vector *image_in2_vec = (vector *) image_in2;
  vector max_vector = INIT_VECTOR(0);
  const unsigned W = width/VECTOR_SIZE;
  const unsigned W2 = in2_stride/VECTOR_SIZE;
  const unsigned off = in2_offset/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
  {
    for (unsigned j = 0; j < W; j++){
      vector v = vector_abs(image_in1_vec[i * W + j] - image_in2_vec[i * W2 + off + j]);
      max_vector = vector_max(v, max_vector);
    }
  }
  max_pix = max_element(max_vector);

#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++) {
    for (unsigned j = 0; j < width; j++) {
      int val = (int)image_in1[i * width + j] - (int)image_in2[i * in2_stride + in2_offset + j];
      if (val < 0) val = -val;
      if (val > max_pix)
        max_pix = val;
    }
  }
#endif

  return max_pix;
}

static pixel min_image(pixel *restrict image_in, unsigned width, unsigned height) {
  pixel min_pix = 0;

#ifdef USE_INTRINSICS
  vector *image_in_vec = (vector *) image_in;
  vector min_vector = INIT_VECTOR(0);
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < W; j++)
      min_vector = vector_min(image_in_vec[i * W + j], min_vector);
  min_pix = min_element(min_vector) ;
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < width; j++)
      if (image_in[i * width + j] < min_pix)
        min_pix = image_in[i * width + j];
#endif

  return min_pix;
}

static void binarisation(pixel *restrict image_in, pixel thresh,
                         pixel val_max, pixel val_min, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
  vector *image_in_vec = (vector *) image_in;
  vector Vth = INIT_VECTOR(thresh-1);
  vector Vones = INIT_VECTOR(1);
  vector C =   INIT_VECTOR(val_max-val_min);
  vector Vmin = INIT_VECTOR(val_min);
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < W; j++) {
      vector V0 = vector_max(image_in_vec[i * W + j], Vth);
      vector V1 = V0 - Vth;
      V0 = vector_min(V1, Vones);
      image_in_vec[i * W + j] = V0 * C + Vmin;
      //image_in_vec[i * W + j] = (vector)_mac4((signed_vector)V0, (signed_vector)C, (signed_vector)Vmin);
    }
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < width; j++)
      if (image_in[i * width + j] < thresh)
        image_in[i * width + j] = val_min;
      else
        image_in[i * width + j] = val_max;
#endif
}

static void erosion(pixel *restrict image_in, pixel *restrict image_out, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
#ifdef SHORT_PIXEL
  vector V11, V12, V21, V22, V31, V32;
  pixel min1;
#ifdef USE_OMP
#pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V21 = *((vector *)&image_in[y-1]);
    V22 = *((vector *)&image_in[y+1]);
    V31 = *((vector *)&image_in[width + y-1]);
    V32 = *((vector *)&image_in[width + y+1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V11 = V21; V12 = V22; V21 = V31; V22 = V32;
      V31 = *((vector *)&image_in[(x+1)*width + y-1]);
      V32 = *((vector *)&image_in[(x+1)*width + y+1]);
      V11 = vector_min(V11, V21);
      V12 = vector_min(V12, V22);
      V11 = vector_min(V11, V31);
      V12 = vector_min(V12, V32);
      min1 = ((V11[1] < V12[0])? V11[1]: V12[0]);
      image_out[x*width + y]   = ((V11[0] < min1)? V11[0]: min1);
      image_out[x*width + y+1] = ((V12[1] < min1)? V12[1]: min1);
    }
  }
#else
  vector V1, V2, V3;
  pixel min1;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V2 = *((vector *)&image_in[y-1]);
    V3 = *((vector *)&image_in[width + y-1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V1 = V2; V2 = V3; V3 = *((vector *)&image_in[(x+1)*width + y-1]);
      V1 = vector_min(V1, V2);
      V1 = vector_min(V1, V3);
      min1 = ((V1[1] < V1[2])? V1[1]: V1[2]);
      image_out[x*width + y]   = ((V1[0] < min1)? V1[0]: min1);
      image_out[x*width + y+1] = ((V1[3] < min1)? V1[3]: min1);
    }
  }
#endif // SHORT_PIXEL
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned x = 1; x < height - 1; x++) {
    for (unsigned y = 1; y < width - 1; y++) {
      pixel min = 255;
      for (unsigned i = 0; i < KERNEL_SIZE; i++)
        for (unsigned j = 0; j < KERNEL_SIZE; j++)
          if (image_in[(x-1) * width + (y-1) + (i*width + j)] < min)
            min = image_in[(x-1) * width + (y-1) + (i*width + j)];

      image_out[x*width + y] = min;
    }
  }
#endif

  /* Boundary Copy */
#if defined(USE_INTRINSICS) && (!defined(USE_TILING) || defined(TILE_ALIGMENT))
  vector *image_in_vec  = (vector *) image_in;
  vector *image_out_vec = (vector *) image_out;
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < W; k++) {
    image_out_vec[k] = image_in_vec[k];
    image_out_vec[(height-1) * W + k] =
        image_in_vec[(height-1) * W + k];
  }
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < width; k++) {
    image_out[k] = image_in[k];
    image_out[(height-1) * width + k] =
        image_in[(height-1) * width + k];
  }
#endif
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < height; k++) {
    image_out[k*width] = image_in[k*width];
    image_out[k*width + width - 1] =
        image_in[k*width + width - 1];
  }
}

static void dilatation(pixel *restrict image_in, pixel *restrict image_out, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
#ifdef SHORT_PIXEL
  vector V11, V12, V21, V22, V31, V32;
  pixel max1;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V21 = *((vector *)&image_in[y-1]);
    V22 = *((vector *)&image_in[y+1]);
    V31 = *((vector *)&image_in[width + y-1]);
    V32 = *((vector *)&image_in[width + y+1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V11 = V21; V12 = V22; V21 = V31; V22 = V32;
      V31 = *((vector *)&image_in[(x+1)*width + y-1]);
      V32 = *((vector *)&image_in[(x+1)*width + y+1]);
      V11 = vector_max(V11, V21);
      V12 = vector_max(V12, V22);
      V11 = vector_max(V11, V31);
      V12 = vector_max(V12, V32);
      max1 = ((V11[1] > V12[0])? V11[1]: V12[0]);
      image_out[x*width + y]   = ((V11[0] > max1)? V11[0]: max1);
      image_out[x*width + y+1] = ((V12[1] > max1)? V12[1]: max1);
    }
  }
#else
  vector V1, V2, V3;
  pixel max1;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V2 = *((vector *)&image_in[y-1]);
    V3 = *((vector *)&image_in[width + y-1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V1 = V2; V2 = V3; V3 = *((vector *)&image_in[(x+1)*width + y-1]);
      V1 = vector_max(V1, V2);
      V1 = vector_max(V1, V3);
      max1 = ((V1[1] > V1[2])? V1[1]: V1[2]);
      image_out[x*width + y]   = ((V1[0] > max1)? V1[0]: max1);
      image_out[x*width + y+1] = ((V1[3] > max1)? V1[3]: max1);
    }
  }
#endif // SHORT_PIXEL
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned x = 1; x < height - 1; x++) {
    for (unsigned y = 1; y < width - 1; y++) {
      pixel max = 0;

      for (unsigned i = 0; i < KERNEL_SIZE; i++)
        for (unsigned j = 0; j < KERNEL_SIZE; j++)
          if (image_in[(x-1) * width + (y-1) + (i*width + j)] > max)
            max = image_in[(x-1) * width + (y-1) + (i*width + j)];

      image_out[x*width + y] = max;
    }
  }
#endif

  /* Boundary Copy */
#if defined(USE_INTRINSICS) && (!defined(USE_TILING) || defined(TILE_ALIGMENT))
  vector *image_in_vec  = (vector *) image_in;
  vector *image_out_vec = (vector *) image_out;
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < W; k++) {
    image_out_vec[k] = image_in_vec[k];
    image_out_vec[(height-1) * W + k] =
        image_in_vec[(height-1) * W + k];
  }
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < width; k++) {
    image_out[k] = image_in[k];
    image_out[(height-1) * width + k] =
        image_in[(height-1) * width + k];
  }
#endif
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < height; k++) {
    image_out[k*width] = image_in[k*width];
    image_out[k*width + width - 1] =
        image_in[k*width + width - 1];
  }
}

static void convolution_rect(pixel *restrict image_in, coeff *restrict kernel,
                             pixel *restrict image_out, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
#ifdef SHORT_PIXEL
  vector V11, V12, V21, V22, V31, V32;
  coeff_vector C11l, C12l, C21l, C22l, C31l, C32l, C11r, C12r, C21r, C22r, C31r, C32r;
  vector temp;
  C11l = *((vector *)kernel); C12l = *((vector *)(kernel+2));
  temp = *((vector *)(kernel+4));
  C21l[0] = C12l[1]; C12l[1] = 0; C21l[1] = temp[0];
  C22l[0] = temp[1]; C22l[1] = 0;
  C31l = *((vector *)(kernel+6)); C32l[0] = kernel[8]; C32l[1] = 0;
  vector mask1 = {3,0};
  vector mask2 = {1,2};
  C11r = __builtin_shuffle(C11l, C12l, mask1);
  C12r = __builtin_shuffle(C11l, C12l, mask2);
  C21r = __builtin_shuffle(C21l, C22l, mask1);
  C22r = __builtin_shuffle(C21l, C22l, mask2);
  C31r = __builtin_shuffle(C31l, C32l, mask1);
  C32r = __builtin_shuffle(C31l, C32l, mask2);
  int val;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V21 = *((vector *)&image_in[y-1]);
    V22 = *((vector *)&image_in[y+1]);
    V31 = *((vector *)&image_in[width + y-1]);
    V32 = *((vector *)&image_in[width + y+1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V11 = V21; V12 = V22; V21 = V31; V22 = V32;
      V31 = *((vector *)&image_in[(x+1)*width + y-1]);
      V32 = *((vector *)&image_in[(x+1)*width + y+1]);
      val = __builtin_pulp_dotsp2(V11, C11l);
      val = __builtin_pulp_sdotsp2(V12, C12l, val);
      val = __builtin_pulp_sdotsp2(V21, C21l, val);
      val = __builtin_pulp_sdotsp2(V22, C22l, val);
      val = __builtin_pulp_sdotsp2(V31, C31l, val);
      val = __builtin_pulp_sdotsp2(V32, C32l, val);
      image_out[x*width + y]  = (val>0? (short)val: (short)-val);
      val = __builtin_pulp_dotsp2(V11, C11r);
      val = __builtin_pulp_sdotsp2(V12, C12r, val);
      val = __builtin_pulp_sdotsp2(V21, C21r, val);
      val = __builtin_pulp_sdotsp2(V22, C22r, val);
      val = __builtin_pulp_sdotsp2(V31, C31r, val);
      val = __builtin_pulp_sdotsp2(V32, C32r, val);
      image_out[x*width + y+1] = (val>0? (short)val: (short)-val);;
    }
  }
#else
  vector V1, V2, V3;
  vector C1l, C2l, C3l, C1r, C2r, C3r;
  C1l = *((vector *)kernel);     C1l[3] = 0;
  C2l = *((vector *)(kernel+3)); C2l[3] = 0;
  C3l = *((vector *)(kernel+6)); C3l[3] = 0;
  vector mask = {3,0,1,2};
  C1r = __builtin_shuffle(C1l, mask);
  C2r = __builtin_shuffle(C2l, mask);
  C3r = __builtin_shuffle(C3l, mask);
  short val;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned y = 1; y < width - 2; y+=2) {
    V2 = *((vector *)&image_in[y-1]);
    V3 = *((vector *)&image_in[width + y-1]);
    for (unsigned x = 1; x < height - 1; x++) {
      V1 = V2; V2 = V3; V3 = *((vector *)&image_in[(x+1)*width + y-1]);
      val = __builtin_pulp_dotsp4(V1, C1l);
      val = __builtin_pulp_sdotsp4(V2, C2l, val);
      val = __builtin_pulp_sdotsp4(V3, C3l, val);
      image_out[x*width + y]   = (val>0? (short)val: (short)-val);;
      val = __builtin_pulp_dotsp4(V1, C1r);
      val = __builtin_pulp_sdotsp4(V2, C2r, val);
      val = __builtin_pulp_sdotsp4(V3, C3r, val);
      image_out[x*width + y+1] = (val>0? (short)val: (short)-val);;
    }
  }
#endif // SHORT_PIXEL
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned x = 1; x < height - 1; x++) {
    for (unsigned y = 1; y < width - 1; y++) {
      int val = 0;
      for (unsigned i = 0; i < KERNEL_SIZE; i++) {
        for (unsigned j = 0; j < KERNEL_SIZE; j++) {
             val += (int)image_in[(x-1) * width + (y-1) +
                    i*width + j] * (int)kernel[i*KERNEL_SIZE + j];

        }
      }
      image_out[x * width + y] = (val>0? (short)val: (short)-val);
    }
  }
#endif

  /* Boundary Copy */
#if defined(USE_INTRINSICS) && (!defined(USE_TILING) || defined(TILE_ALIGMENT))
  vector *image_in_vec  = (vector *) image_in;
  vector *image_out_vec = (vector *) image_out;
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < W; k++) {
    image_out_vec[k] = image_in_vec[k];
    image_out_vec[(height-1) * W + k] =
        image_in_vec[(height-1) * W + k];
  }
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < width; k++) {
    image_out[k] = image_in[k];
    image_out[(height-1) * width + k] =
        image_in[(height-1) * width + k];
  }
#endif

#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned k = 0; k < height; k++) {
    image_out[k*width] = image_in[k*width];
    image_out[k*width + width - 1] =
        image_in[k*width + width - 1];
  }

}

static void multiply(pixel *restrict image_in1, pixel *restrict image_in2, unsigned width, unsigned height) {
#ifdef USE_INTRINSICS
  vector *image_in1_vec = (vector *) image_in1;
  vector *image_in2_vec = (vector *) image_in2;
  const unsigned W = width/VECTOR_SIZE;
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < W; j++)
      image_in1_vec[i * W + j] *= image_in2_vec[i * W + j];
#else
#ifdef USE_OMP
  #pragma omp for schedule(static)
#endif
  for (unsigned i = 0; i < height; i++)
    for (unsigned j = 0; j < width; j++)
      image_in1[i*width + j] =
            image_in1[i*width + j] * image_in2[i*width + j];
#endif
}

static int check_image(pixel *restrict output, pixel *restrict golden, unsigned width, unsigned height) {
  unsigned errors = 0;

  for (unsigned i = 0; i < height * width; i++)
  if (output[i] != golden[i]) {
      errors++;
      printf("Error@%d -> %d |= %d\n", i, output[i], golden[i]);
  }

  /* if (errors == 0) */
  /*   printf("OOOOOOK!!!!!!\n"); */
  /* else */
  /*   printf("ERROR!!!! %d\n", errors); */
  return errors;
}

static void print_image(pixel *restrict test, unsigned width, unsigned height) {
  for (unsigned y = 0; y < height; y++) {
    for (unsigned x = 0; x < width; x++)
      printf("%3x", (unsigned char) test[width * y + x]);
    printf("\n");
  }

  printf("\n");
}


// L2 data
coeff sobel1_l2[9] = {1,2,1,0,0,0,-1,-2,-1};
coeff sobel2_l2[9] = {1,0,-1,2,0,-2,1,0,-1};
pixel result[IMG_HEIGHT][IMG_WIDTH];

// L1 data
GAP_L1_DATA  coeff sobel1[9];
GAP_L1_DATA  coeff sobel2[9];
GAP_L1_DATA  pixel image_back[IMG_HEIGHT][IMG_WIDTH];
#ifdef USE_TILING
GAP_L1_DATA  pixel (*image_in)[BUFFER_WIDTH];
GAP_L1_DATA  pixel (*image_out)[BUFFER_WIDTH];
GAP_L1_DATA  pixel image_in_buffers[2][BUFFER_HEIGHT][BUFFER_WIDTH];
GAP_L1_DATA  pixel image_out_buffers[2][TILE_HEIGHT][TILE_WIDTH];
#else
GAP_L1_DATA  pixel image_in[BUFFER_HEIGHT][BUFFER_WIDTH];
GAP_L1_DATA  pixel image_out[BUFFER_HEIGHT][BUFFER_WIDTH];
#endif
GAP_L1_DATA  pixel image_temp0[BUFFER_HEIGHT][BUFFER_WIDTH];
GAP_L1_DATA  pixel image_temp1[BUFFER_HEIGHT][BUFFER_WIDTH];
GAP_L1_DATA  pixel image_temp2[BUFFER_HEIGHT][BUFFER_WIDTH];


int main(int argc, char *argv[]) {

  int time = 0;
  int max_pixel = 0;
  unsigned int width = IMG_WIDTH;
  unsigned int height = IMG_HEIGHT;
  dma_req_t input_job_id;
  dma_req_t output_job_id[2];
#ifdef USE_OMP
  int max_local[NUM_CORES];
#endif
#ifdef USE_TILING
  unsigned int buffer_id = 0, next_buffer_id;
  unsigned int tile_offset[N_TILES], out_tile_offset[N_TILES],
               tile_width[N_TILES], tile_height[N_TILES],
               l_border[N_TILES], t_border[N_TILES];
#endif

  /* printf("Initializing Motion Detection Application Data Structures... \n"); */

#ifdef USE_TILING
  for(unsigned i=0, k=0; i<H_TILES; ++i) {
    for(unsigned j=0; j<W_TILES; ++j) {
      t_border[k]  = (i>0? BORDER_SIZE: 0);
      l_border[k]  = (j>0? BORDER_SIZE: 0);
      unsigned int w_borders  = (j>0? BORDER_SIZE: 0) + (j<W_TILES-1? BORDER_SIZE: 0);
      unsigned int h_borders  = (i>0? BORDER_SIZE: 0) + (i<H_TILES-1? BORDER_SIZE: 0);
      tile_offset[k] = (IMG_WIDTH*(TILE_HEIGHT-t_border[k]))*i +
                       TILE_WIDTH*j - l_border[k];
      out_tile_offset[k] =  (IMG_WIDTH*TILE_HEIGHT)*i + TILE_WIDTH*j;
      tile_width[k]  = TILE_WIDTH + w_borders;
      tile_height[k] = TILE_HEIGHT + h_borders;
      //printf("(%d, %d) -> %d (%d x %d)\n", i, j, tile_offset[k], tile_width[k], tile_height[k]);
      ++k;
    }
  }
#endif

  // INITIALIZING SOBEL VECTORS
  for (unsigned i = 0; i < 9; i++) {
    sobel1[i] = sobel1_l2[i];
    sobel2[i] = sobel2_l2[i];
  }

  // INITIALIZING BACKGROUND
  memcpy_async(image_back, Y_back, IMG_WIDTH*IMG_HEIGHT*sizeof(pixel), &input_job_id);
  // INITIALIZING IMAGE
#ifdef USE_TILING
  // Get first input tile (wait)
  memcpy_async2D(&image_in_buffers[0][0][0], (pixel *)Y_test,
                 tile_width[0]*tile_height[0]*sizeof(pixel),
                 IMG_WIDTH*sizeof(pixel), tile_width[0]*sizeof(pixel),
                 &input_job_id);
  memcpy_wait(&input_job_id);
  // Get second input tile (no wait)
  memcpy_async2D(&image_in_buffers[1][0][0],
                 (pixel *)Y_test+tile_offset[1],
                 tile_width[1]*tile_height[1]*sizeof(pixel),
                 IMG_WIDTH*sizeof(pixel), tile_width[1]*sizeof(pixel),
                 &input_job_id);
#else
  // Get full input image
  memcpy_async(image_in, Y_test, IMG_WIDTH*IMG_HEIGHT*sizeof(pixel), &input_job_id);
  memcpy_wait(&input_job_id);
#endif

#ifdef APP_DEBUG
  print_image((pixel *)image_back, IMG_WIDTH, IMG_HEIGHT);
  print_image((pixel *)Y_test, IMG_WIDTH, IMG_HEIGHT);
#endif

  /* printf("Starting Motion Detection Application \n"); */

  for(int iter = 0; iter < 3; ++iter)
  {
#ifdef PROFILING
  profile_start(iter);
#endif

#ifdef USE_TILING
// Tiling loop
for(unsigned k=0; k<N_TILES; ++k) {
  // Tile size
  width  = tile_width[k];
  height = tile_height[k];
  // Double buffering status
  image_in  = image_in_buffers[buffer_id];
  image_out = image_out_buffers[buffer_id];
  //printf(">>>>>>>>>> IMAGE IN (k = %d) \n", k);
  //print_image((pixel *)image_in, width, height);

#ifdef PRECISE_TILING

#ifdef USE_OMP
#pragma omp parallel num_threads(NUM_CORES)
{
  max_local[omp_get_thread_num()] = max_sub_image((pixel *)image_in, (pixel *)image_back,
                                    width, height, IMG_WIDTH, tile_offset[k]);;
}
  for(unsigned i=0; i<NUM_CORES; ++i)
    max_pixel = ((max_pixel > max_local[i])? max_pixel: max_local[i]);
#else
  int new_val = max_sub_image((pixel *)image_in, (pixel *)image_back,
                               width, height, IMG_WIDTH, tile_offset[k]);
  if(new_val > max_pixel) max_pixel = new_val;
#endif

  next_buffer_id = (buffer_id==0? 1: 0);
  if(k < N_TILES-1) memcpy_wait(&input_job_id);
  if(k < N_TILES-2) {
     memcpy_async2D(&image_in_buffers[buffer_id][0][0],
                     (pixel *)Y_test+tile_offset[k+2],
                     tile_width[k+2]*tile_height[k+2]*sizeof(pixel),
                     IMG_WIDTH*sizeof(pixel), tile_width[k+2]*sizeof(pixel),
                     &input_job_id);
  }

  buffer_id = next_buffer_id;
}

  // Get first input tile (wait)
  memcpy_async2D(&image_in_buffers[0][0][0], (pixel *)Y_test,
                 tile_width[0]*tile_height[0]*sizeof(pixel),
                 IMG_WIDTH*sizeof(pixel), tile_width[0]*sizeof(pixel),
                 &input_job_id);
  memcpy_wait(&input_job_id);
// Get second input tile (no wait)
  memcpy_async2D(&image_in_buffers[1][0][0],
                 (pixel *)Y_test+tile_offset[1],
                 tile_width[1]*tile_height[1]*sizeof(pixel),
                 IMG_WIDTH*sizeof(pixel), tile_width[1]*sizeof(pixel),
                 &input_job_id);

  buffer_id = 0;
for(unsigned k=0; k<N_TILES; ++k) {
  // Tile size
  width  = tile_width[k];
  height = tile_height[k];
  // Double buffering status
  image_in  = image_in_buffers[buffer_id];
  image_out = image_out_buffers[buffer_id];

#endif // PRECISE_TILING


#endif

#ifdef USE_OMP
#ifdef USE_TILING
#pragma omp parallel num_threads(NUM_CORES) default(shared) firstprivate(width, height, buffer_id, k)
#else
#pragma omp parallel num_threads(NUM_CORES) default(shared) firstprivate(width, height)
#endif
{
#endif


#ifdef APP_DEBUG
  printf("Sub image \n");
#endif

#ifdef USE_TILING
  sub_image((pixel *)image_in, (pixel *)image_back, (pixel *)image_temp0,
            width, height, IMG_WIDTH, tile_offset[k]);
#else
  sub_image((pixel *)image_in, (pixel *)image_back, (pixel *)image_temp0,
            width, height, IMG_WIDTH, 0);
  //val_abs((pixel*) image_temp0);
#endif

#ifndef PRECISE_TILING
#ifdef USE_OMP
  max_local[omp_get_thread_num()] = max_image((pixel *)image_temp0, width, height);
  #pragma omp single
  {
    for(unsigned i=0; i<NUM_CORES; ++i)
      max_pixel = ((max_pixel > max_local[i])? max_pixel: max_local[i]);
  }

#else
    max_pixel = max_image((pixel *)image_temp0, width, height);
#endif
#endif

#ifdef APP_DEBUG
  print_image((pixel *)image_temp0, width, height);
  printf("Max pixel: %x \n", max_pixel);
  printf("Binarization \n");
#endif

  binarisation((pixel *)image_temp0, (max_pixel * 3 / 10), 1, 0, width, height);

#ifdef APP_DEBUG
  print_image((pixel *)image_temp0, width, height);
  printf("Erosion \n");
#endif

  erosion((pixel *)image_temp0, (pixel *)image_temp1, width, height);

#ifdef APP_DEBUG
  print_image((pixel *)image_temp1, width, height);
  printf("Dilatation \n");
#endif

  dilatation((pixel *)image_temp1, (pixel *)image_temp2, width, height);

#ifdef APP_DEBUG
  print_image((pixel *)image_temp2, width, height);
  printf("Sobel Convolution \n");
#endif

  convolution_rect((pixel *)image_temp2, sobel1, (pixel *)image_temp0, width, height);
  //val_abs((pixel*) image_temp0, width, height);

  convolution_rect((pixel *)image_temp2, sobel2, (pixel *)image_temp1, width, height);
  //val_abs((pixel*) image_back, width, height);

  sum_image((pixel *)image_temp0, (pixel *)image_temp1, width, height);
  binarisation((pixel *)image_temp0, 1, 0, 1, width, height);

#ifdef APP_DEBUG
    print_image((pixel *)image_temp0, width, height);
    printf("Final Multiplication \n");
#endif

  multiply((pixel *)image_temp0, (pixel *)image_in, width, height);

#ifdef APP_DEBUG
    print_image((pixel *)image_temp0, width, height);
#endif

#ifdef USE_TILING
  for(unsigned i = 0; i<TILE_HEIGHT; ++i)
    for(unsigned j = 0; j<TILE_WIDTH; ++j)
      image_out_buffers[buffer_id][i][j] = image_temp0[t_border[k]+i][l_border[k]+j];
#endif

#ifdef USE_OMP
} // parallel region
#endif

#ifdef USE_TILING
  next_buffer_id = (buffer_id==0?1: 0);
  if(k < N_TILES-1) memcpy_wait(&input_job_id);
  if(k > 0) memcpy_wait(&output_job_id[next_buffer_id]);
  if(k < N_TILES-2) {
     memcpy_async2D(&image_in_buffers[buffer_id][0][0],
                     (pixel *)Y_test+tile_offset[k+2],
                     tile_width[k+2]*tile_height[k+2]*sizeof(pixel),
                     IMG_WIDTH*sizeof(pixel), tile_width[k+2]*sizeof(pixel),
                     &input_job_id);
  }
  memcpy_async2D((pixel *)result+out_tile_offset[k],
                 &image_out_buffers[buffer_id][0][0],
                 TILE_WIDTH*TILE_HEIGHT*sizeof(pixel),
                 IMG_WIDTH*sizeof(pixel), TILE_WIDTH*sizeof(pixel),
                 &output_job_id[buffer_id]);
  buffer_id = next_buffer_id;
  } // tiling loop
  memcpy_wait(&output_job_id[1]);
#else
  memcpy_async(result, image_temp0, IMG_WIDTH*IMG_HEIGHT*sizeof(pixel), &output_job_id[0]);
  memcpy_wait(&output_job_id[0]);
#endif

#ifdef PROFILING
  profile_stop(iter);
#endif

#ifdef CHECK
  if (check_image((pixel *)result, (pixel *)Y_golden, width, height)) return -1;
#endif
  }//iter
  profile_show();

  /* printf("Motion Detection Application Complete!!!\n"); */

  return 0;
}
