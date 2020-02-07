/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __IMG_IO_H__
#define __IMG_IO_H__

#include "fs_switch.h"

int ReadImageFromFile(char *ImageName, unsigned int DesiredW, unsigned int DesiredH, unsigned int DesiredBytesPerPixel, void *InBuffer, unsigned int BuffSize, int shift, int shorts);
int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *InBuffer);
#endif //__IMG_IO_H__

