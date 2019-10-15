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


void LoadResizeLibrary();

void ResizeConfiguration(
	unsigned int L1Memory
	);
void GenerateResize(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout);

#endif //__RESIZE_GENERATOR_H__
