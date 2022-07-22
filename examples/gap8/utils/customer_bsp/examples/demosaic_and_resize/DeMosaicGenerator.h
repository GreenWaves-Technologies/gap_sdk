/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#ifndef __DEMOSAIC_GENERATOR_H__
#define __DEMOSAIC_GENERATOR_H__


typedef enum {
	GRBG,
    GBRG,
    BGGR  
} BayerOrder_t;


void LoadDeMosaicLibrary();

void DeMosaicConfiguration(unsigned int L1Memory,unsigned int L2Memory);


//This Generator average each 2x2 image pixel size to generate a color image which is W/2 x H/2 x 3C
//This could be used to have best image quality. 
int Simple_DeMosaic_Resize(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt);

//This Generator could be used to produce Channel First images, which is good when we need to feed algortihms with
int GenerateDeMosaic_OutCHW(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt);

//This Generator could be used to produce Channel Last images, which is good when we need to save them to host
//int GenerateDeMosaic_OutWHC(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt);
#endif //__DEMOSAIC_GENERATOR_H__
