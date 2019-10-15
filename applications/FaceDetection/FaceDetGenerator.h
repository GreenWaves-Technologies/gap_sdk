/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __RESIZE_GENERATOR_H__
#define __RESIZE_GENERATOR_H__


void LoadFaceDetectionLibrary();

void FaceDetectionConfiguration(
	unsigned int L1Memory
	);
void GenerateResize(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout);

void GenerateIntegralImage(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H 		/* Image Height */
	);

void GenerateSquaredIntegralImage(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H 		/* Image Height */
	);

void GenerateCascadeClassifier(char *Name,
		unsigned int W,     /* Image width */
		unsigned int H,     /* Image Height */
		unsigned int WinW,  /* Detection window width */
		unsigned int WinH   /* Detection window Height */
	);

#endif //__RESIZE_GENERATOR_H__
