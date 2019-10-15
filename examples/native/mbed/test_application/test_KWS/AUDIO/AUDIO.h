#include <stdio.h>
#include <string.h>
#include "gap_sai.h"

#define WAV_HEADER_SIZE 44//in bytes
// 65408 16b samples @ 22.05kHz eq. to 2.99/2=1.495 sec of audio record in stereo
#define BUFFER_SIZE  ((32768*2) - 128) //in bytes

extern uint8_t* i2s_buffer;

void Audio_Init();
void Audio_Enable();
void Audio_Capture();
void Audio_Disable();
void Audio_DeInit();
void Audio_SynchroPython();
