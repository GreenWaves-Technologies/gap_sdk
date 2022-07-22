#ifndef __CNN_FLOAT_TYPE_H__
#define __CNN_FLOAT_TYPE_H__

#ifdef __EMUL__
        #ifdef __FLOAT_EMUL__
                typedef float F16;
                typedef float F16V __attribute__((vector_size (8)));
                typedef int V2S __attribute__((vector_size (8)));
        #else
                typedef short int F16;
                typedef short int F16V __attribute__((vector_size (4)));
                typedef v2s V2S;
        #endif
        typedef union {
                F16     S;
                F16V    V;
        } F16SV;
#else
        typedef v2s V2S;
        /* GAP9 */
        #ifdef __gap9__
                #ifdef STD_FLOAT
                        typedef float16 F16;
                        typedef v2h F16V;
                #else
                        typedef float16alt F16;
                        typedef v2ah F16V;
                #endif
                typedef union {
                        F16     S;
                        F16V    V;
                } F16SV;
                #define TO_FLOAT(x) x
        /* GAP8 */
        #else
                typedef short int F16;
                typedef short int F16V __attribute__((vector_size (4)));
                typedef union {
                        F16     S;
                        F16V    V;
                } F16SV;
        #endif // gap9
#endif // __EMUL__

#endif

