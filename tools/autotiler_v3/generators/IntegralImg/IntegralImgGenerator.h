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

#ifndef __II_GENERATOR_H__
#define __II_GENERATOR_H__

/** @addtogroup groupIntegralImage
@{ */


void LoadIntegralImageLibrary();

void IntegralImageConfiguration(
	unsigned int L1Memory	/**< Maximum amount of memory in byte usable by the AutoTiler in shared L1 */
	);

void GenerateIntegralImage(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H 		/* Image Height */
	);

/** @} */
#endif //__II_GENERATOR_H__
