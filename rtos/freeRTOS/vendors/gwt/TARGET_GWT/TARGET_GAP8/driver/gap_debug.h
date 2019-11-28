/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * Copyright (c) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GAP_DEBUG_H_
#define _GAP_DEBUG_H_

#include <stdint.h>

#define HAL_DEBUG_STRUCT_NAME Debug_Struct
#define HAL_DEBUG_STRUCT_NAME_STR "Debug_Struct"

#define PRINTF_BUF_SIZE 128


typedef struct {
  volatile int32_t connected;
} __attribute__((packed)) target_state_t;

typedef struct {
  volatile int32_t connected;
} __attribute__((packed)) bridge_state_t;

/* This structure can be used to interact with the host loader */
typedef struct _debug_struct {
    uint32_t version;

    target_state_t target;

    bridge_state_t bridge;

    /* Used by external debug bridge to get exit status when using the board */
    uint32_t exitStatus;

    /* Printf */
    uint32_t useInternalPrintf;
    uint32_t putcharPending;
    uint32_t putcharCurrent;
    uint8_t putcharBuffer[PRINTF_BUF_SIZE];

    /* Debug step, used for showing progress to host loader */
    uint32_t debugStep;
    uint32_t debugStepPending;

    // Requests
    uint32_t firstReq;
    uint32_t lastReq;
    uint32_t firstBridgeReq;
    uint32_t firstBridgeFreeReq;
    uint32_t targetReq;

    uint32_t notifyReqAddr;
    uint32_t notifyReqValue;

} __attribute__((packed)) debug_struct_t;

extern debug_struct_t HAL_DEBUG_STRUCT_NAME;

static inline debug_struct_t* DEBUG_GetDebugStruct()
{
#ifdef GAP_USE_DEBUG_STRUCT
  return &HAL_DEBUG_STRUCT_NAME;
#else
  return (void *)0;
#endif
}

#define GAP_DEBUG_STRUCT_INIT { PROTOCOL_VERSION, {0}, {0}, 0, 1, 0 ,0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0}

static inline void DEBUG_FlushPrintf(debug_struct_t *debugStruct) {
  while(*(volatile uint32_t *)&debugStruct->putcharPending);
}

static inline void DEBUG_Exit(debug_struct_t *debugStruct, int status) {
  *(volatile uint32_t *)&debugStruct->exitStatus = 0x80000000 | status;
}

static inline int DEBUG_IsEmpty(debug_struct_t *debugStruct)
{
  return *(volatile uint32_t *)&debugStruct->putcharCurrent == 0;
}

static inline int DEBUG_IsBusy(debug_struct_t *debugStruct)
{
  return *(volatile uint32_t *)&debugStruct->putcharPending;
}

static inline void DEBUG_SendPrintf(debug_struct_t *debugStruct) {
  if (debugStruct->putcharCurrent)
  {
    *(volatile uint32_t *)&debugStruct->putcharPending = debugStruct->putcharCurrent;
    *(volatile uint32_t *)&debugStruct->putcharCurrent = 0;
  }
}

static inline void DEBUG_Putchar(debug_struct_t *debugStruct, char c) {
  DEBUG_FlushPrintf(debugStruct);
  *(volatile uint8_t *)&(debugStruct->putcharBuffer[debugStruct->putcharCurrent++]) = c;
  if (*(volatile uint32_t *)&debugStruct->putcharCurrent == PRINTF_BUF_SIZE || c == '\n') {
    *(volatile uint32_t *)&debugStruct->putcharPending = debugStruct->putcharCurrent;
    *(volatile uint32_t *)&debugStruct->putcharCurrent = 0;
  }
}

static inline int DEBUG_PutcharNoPoll(debug_struct_t *debugStruct, char c) {
  if (*(volatile uint32_t *)&debugStruct->putcharPending)
    return -1;

  *(volatile uint8_t *)&(debugStruct->putcharBuffer[debugStruct->putcharCurrent++]) = c;
  if (*(volatile uint32_t *)&debugStruct->putcharCurrent == PRINTF_BUF_SIZE || c == '\n') {
    DEBUG_SendPrintf(debugStruct);
  }
  return 0;
}

static inline void DEBUG_Step(debug_struct_t *debugStruct, uint32_t value) {
  *(volatile uint32_t *)&debugStruct->debugStep = value;
  *(volatile uint32_t *)&debugStruct->debugStepPending = 1;
  while (*(volatile uint32_t *)&debugStruct->debugStepPending);
}

static inline void DEBUG_Reset(debug_struct_t *debugStruct) {
  *(volatile uint32_t *)&debugStruct->exitStatus = 0x80000000 | 0x40000000;
}

#endif  /* _GAP_DEBUG_H_ */
