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

#if defined(__PULP_OS__)
#include "bridge_stubs.h"
#endif  /* __PULP_OS__ */


unsigned char *ReadImageFromFile(char *ImageName, unsigned int *W, unsigned int *H, unsigned char *InBuffer, unsigned int BuffSize);
int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *InBuffer);
#endif //__IMG_IO_H__

