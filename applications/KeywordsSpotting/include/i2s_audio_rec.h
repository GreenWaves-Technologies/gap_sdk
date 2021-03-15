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

#ifndef __I2S_AUDIO_REC__
#define __I2S_AUDIO_REC_

#define WAV_HEADER_SIZE 44//in bytes
// 65408 16b samples @ 22.05kHz eq. to 2.99/2=1.495 sec of audio record in stereo
#define BUFFER_SIZE  ((16384*2)+WAV_HEADER_SIZE) //in bytes

typedef struct {
	unsigned int	l2Addr;
	unsigned int 	size;
	unsigned int	recReady;
	unsigned int	micReady;
  unsigned int  cStarted;
} I2sDescriptor;

extern RT_L2_DATA I2sDescriptor I2sOutHeader;
extern RT_L2_DATA unsigned char i2s_buffer_ch0[(BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE];

short int * sampling();
void printCfg(unsigned int i2s);
void CreateWAVHeader(unsigned char *I2sOut, I2sDescriptor *I2sOutHeader);

#endif
