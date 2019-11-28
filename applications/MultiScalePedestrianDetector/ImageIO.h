/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
