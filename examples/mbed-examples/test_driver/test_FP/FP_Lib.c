#include "FP_Lib.h"

char *__FP(int X, int Prec, char *Out)
{
    char Im[10];
    int Sign = (X<0);
    int Int, Fract;
    int DigPrec=100000;
    int i, IntDig, FractDig=5, V;
    int Ind=0;

    if (Sign) X = -X;
    Int = X>>Prec;
    Fract = X - (Int<<Prec);
    V = (Fract*DigPrec)/(1<<Prec);
    for (i=0; i<FractDig; i++) {
        Im[i] = V%10; V = V/10;
    }
    IntDig=0;
    do {
        Im[5+IntDig] = Int%10; Int = Int/10; IntDig++;
    } while (Int);
    if (Sign) Out[Ind++] = '-';
    for (i=IntDig+FractDig-1;i>=FractDig;i--) Out[Ind++] = '0' + Im[i];
    Out[Ind++] = '.';
    for (i=FractDig-1; i>=0; i--) Out[Ind++] = '0' + Im[i];
    Out[Ind] = 0;
    return Out;
}
