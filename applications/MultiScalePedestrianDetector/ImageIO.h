#ifndef _IMAGEIO_H_
#define _IMAGEIO_H_
#include "DataTypes.h"


void writePPM(const char *filename, Pixu *img, int W, int H, int isRGB);
void writePaddedPPM(const char *filename, Pixu *img, int W, int H, int PadW, int PadH, int isRGB);
void writePPM_HR(const char *filename, Pixuw *img, int W, int H, int isRGB);
Pixu *Read_Ppm_Pgm_Image(char *file_name, int *W, int *H, int RetRGB, unsigned char *InBuffer, unsigned int BuffSize);
Pixu *Read_Padded_Ppm_Pgm_Image(char *file_name, int *W, int *H, int RetRGB, int PadW, int PadH, int PadRule);
void PadImage(Pixu *Img, int W, int H, int IsRGB, int PadW, int PadH, int PadRule, Pixu *FromImg);
Pixu *RGBto4Channels(Pixu *In, int W, int H);
Pixu *RGBtoYUV(Pixu *In, int W, int H);



#ifdef WITH_PNG
Pixu *Read_Png_Image(const char *file_name, int *W, int *H, int AsRGB);
#endif

#ifndef __pulp
char **GetAllFileNamesInDirectory(char *DirectoryName, char *Extension, int *N);
#endif


#endif
