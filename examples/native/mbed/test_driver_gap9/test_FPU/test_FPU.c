#include <stdio.h>
#include <math.h>
#include "cmsis.h"
#include "FP_Lib.h"

int Prec = 10;
char Out[10];

#define F2Q(V, N)       ((int) roundf(((float) (V))*((1<<(N))-0)))

int main()
{
    int error = 0;

    volatile float a = 49.0;
    volatile float b = 5.0 ;
    volatile float c = 35.0;
    volatile float d = 10.0;
    volatile float e;
    volatile float f;

    e = c+d;
    if(e!=45.0F)
    {
        error++;
    }
    __FP(F2Q(e, Prec), Prec, Out);    printf("%s\n", Out);

    e = c-d;
    if(e!=25.0F)
    {
        error++;
    }
    __FP(F2Q(e, Prec), Prec, Out);    printf("%s\n", Out);

    e = c*d;
    if(e!=350.0F)
    {
        error++;
    }
    __FP(F2Q(e, Prec), Prec, Out);    printf("%s\n", Out);

    f = a/b;
    if(f!= 9.8F)
    {
        error++;
    }
    __FP(F2Q(f, Prec), Prec, Out);    printf("%s\n", Out);

    f = sqrtf(a);
    if(f!=7.0F)
    {
        error++;
    }
    __FP(F2Q(f, Prec), Prec, Out);    printf("%s\n", Out);

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
