#ifndef DATA_TYPES_INC

#define DATA_TYPES_INC

typedef signed char    Pixs;
typedef unsigned char  Pixu;
typedef unsigned short Pixuw;
typedef signed   short Pixsw;
typedef unsigned char  PixuV __attribute__((vector_size (4)));
typedef unsigned short PixuVw __attribute__((vector_size (4)));
typedef signed char    PixsV __attribute__((vector_size (4)));
typedef signed short   PixsVw __attribute__((vector_size (4)));



typedef signed char   Filts;
typedef unsigned char FiltuV __attribute__((vector_size (4)));
typedef signed char   FiltsV __attribute__((vector_size (4)));

typedef signed short  WeightT;

// Scales must be properly sorted:
// Downscales from bigger to smaller
// Upscales from smaller to bigger

typedef struct pyramid_scales{
        float *       real_scales;
        float *       approx_scales;
        unsigned char num_real_scales;
        unsigned char num_approx_scales;
        //upscales
        float *       real_up_scales;
        float *       approx_up_scales;
        unsigned char num_real_up_scales;
        unsigned char num_approx_up_scales;

}t_pyramid_scales;

typedef struct detected_wins{
        short*          loc_x;
        short*          loc_y;
        unsigned short*   w_x;
        unsigned short*   w_y;
        float*          score;
        float           scale;

        unsigned short num_det;
}t_detected_wins;

#define MAX_LINE 4000

#endif
