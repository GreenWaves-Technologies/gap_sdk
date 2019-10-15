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

#define MAX_LINE 4000

#endif
