#include <stdio.h>
#include <math.h>
#include "FP_Lib.h"
#include "gap_common.h"

#define F2Q(V, N)       ((int) roundf(((float) (V))*((1<<(N))-0)))

int main() {

    int Prec = 10;
    char Out[10];

    __FP(F2Q(0.5, Prec), Prec, Out);    printf("%s\n", Out);
    __FP(F2Q(0.001, Prec), Prec, Out);  printf("%s\n", Out);
    __FP(F2Q(-0.001, Prec), Prec, Out); printf("%s\n", Out);
    __FP(F2Q(-0.5, Prec), Prec, Out);   printf("%s\n", Out);

    printf("Test success\n");

    return 0;
}
