/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __MATMGEN_H__
#define __MATMGEN_H__
extern void LoadMatMultLibrary();
extern void ParMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2, int Type);
extern void ParVectMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2, int Type);
#endif
