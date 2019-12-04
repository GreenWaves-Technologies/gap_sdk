#include "print_float.h"
#include <stdio.h>

float fAbs(float x){

    return ((x<0.0F) ? (-x) : (x));
}
void printFloat(float f)
{
  int j = 100000000;
  int i = j;
  float fi;
  char sign;
  unsigned int * ph = (unsigned int *)&f;
  unsigned int h = *ph;
  sign = (f<0.0)?'-':'+';


  f    = fAbs(f);
  fi   = (float) i;
  i    = (int) f;
  i    = (int) ((fi * f) - ((float)i) * fi);

  if (i < 0)
  {
    i += j;
    j  = ((int)f) - 1;
  }
  else
  {
    j = (int)f;
  }

  printf("%c%d.%08d",sign,j,i);
//  printf("%c%d.%08d (mantissa) %d exponent %d",sign,j,i, h & 0x7FFFF,  h & 0x7F800000 >> 23);
  return;
}
