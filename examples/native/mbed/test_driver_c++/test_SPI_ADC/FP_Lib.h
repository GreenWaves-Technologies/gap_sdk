#ifndef __FP_LIB_H__
#define __FP_LIB_H__

#include <math.h>

#define F2Q(V, N)       ((int) roundf(((float) (V))*((1<<(N))-0)))

char *__FP(int X, int Prec, char *Out);

void printf_fp(float input);

#endif
