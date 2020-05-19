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


#ifndef __IMG_IO_H__
#define __IMG_IO_H__

#define PPM_HEADER                          40

unsigned char *ReadImageFromFile(char *ImageName, unsigned int *W, unsigned int *H, unsigned char *InBuffer, unsigned int BuffSize);
int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *InBuffer, unsigned char PixelSize);
#endif //__IMG_IO_H__

