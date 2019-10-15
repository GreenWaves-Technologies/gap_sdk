/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __RESIZE_GENERATOR_H__
#define __RESIZE_GENERATOR_H__

/** @addtogroup groupResize
@{ */

/**
 @brief Load Resizing Basic Kernels

 Load Resizing Basic Kernels.
*/
void LoadResizeLibrary();

void ResizeConfiguration(
	unsigned int L1Memory
	);

/**
@brief Generate Resizing for a given set of parameters

Generate Resizing for a given set of parameters

    \param    Name:           Name of the generated user kernel
    \param    Win :           Width of the input
    \param    Hin :           Hight of the input
    \param    Wout:           Width of the output
    \param    Hout:           Hight of the output
*/
void GenerateResize(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout);

/** @} */
#endif //__RESIZE_GENERATOR_H__
