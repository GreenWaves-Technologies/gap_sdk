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

typedef enum {
	IMGIO_OUTPUT_CHAR,
	IMGIO_OUTPUT_SHORT,
	IMGIO_OUTPUT_RGB565
} img_io_out_t;

enum {
    BYPASS_IO = 0,
    GRAY_SCALE_IO = 1,
    RGB565_IO = 2,
    RGB888_IO = 3
};

typedef enum {
	IMGIO_SHIFT = 1
} img_io_post_process_t;

int ReadImageFromFile(char *ImageName, unsigned int DesiredW, unsigned int DesiredH, unsigned int DesiredBytesPerPixel, void *InBuffer, unsigned int BuffSize, img_io_out_t out_type, int Transpose2CHW);
int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char PixelSize, unsigned char *OutBuffer, unsigned char imgFormat);


#endif //__IMG_IO_H__

