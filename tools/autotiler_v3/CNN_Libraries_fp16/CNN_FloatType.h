#ifndef __CNN_FLOAT_TYPE_H__
#define __CNN_FLOAT_TYPE_H__

#ifdef __EMUL__
typedef short int F16;
typedef short int F16V __attribute__((vector_size (4)));
typedef union {
        F16     S;
        F16V    V;
} F16SV;
#else
#ifdef __gap9__
typedef float16alt F16;
typedef float16alt F16V __attribute__((vector_size (4)));
typedef union {
        F16     S;
        F16V    V;
} F16SV;
#else
typedef short int F16;
typedef short int F16V __attribute__((vector_size (4)));
typedef union {
        F16     S;
        F16V    V;
} F16SV;
#endif
#endif

#endif

