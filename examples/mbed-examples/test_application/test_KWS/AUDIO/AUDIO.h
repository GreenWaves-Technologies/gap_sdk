#include <stdio.h>
#include <string.h>
#include "gap_sai.h"

#define WAV_HEADER_SIZE 44//in bytes
// 65408 16b samples @ 22.05kHz eq. to 2.99/2=1.495 sec of audio record in stereo
#define BUFFER_SIZE  ((16384*2)+WAV_HEADER_SIZE) //in bytes

extern uint8_t i2s_buffer[(BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE];

void Audio_Capture();
void Audio_Init();
