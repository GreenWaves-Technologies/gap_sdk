/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/* 
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef _JPEG_ENCODER_H_
#define _JPEG_ENCODER_H_

//#define DUMP_DU 1

#define EOB 0xA
#define EOB_FLIP 0x5
#define EOB_LEN 4
#define ZRL 0x7F9
#define ZRL_FLIP 0x4FF
#define ZRL_LEN 11

//#define MAX_BYTES_IN_DU   216 /* 64*(11 + 16) => 1728 bits 0> 216 bytes */
#define MAX_BYTES_IN_DU 260 /* 65*(16)*2 => 2080 bits 0> 260 bytes */

#ifdef DUMP_DU
static void dump_du(const char *msg, short *du)
{
    if (pi_cluster_id() == 0)
      pi_cl_team_critical_enter();
    printf("%ld DU %s\n", pi_core_id(), msg);

    int i;
    for (i=0; i<64; i++) {
        printf("%3d%c", du[i], i % 8 == 7 ? '\n' : ' ');
    }
    printf("\n");

    if (pi_cluster_id() == 0)
      pi_cl_team_critical_exit();
}
#else
#define dump_du(x...)
#endif

static inline void SizeAndIndex(int X, int *SymbolSize, int *SymbolIndex)

{
    int AbsX = (X < 0) ? -X : X;
    int Size = X == 0 ? 0 : 32 - __builtin_clz(AbsX);
    int Sign = (X < 0);

    *SymbolSize = Size;
    *SymbolIndex = Sign * ((1 << Size) - 1) + (1 - 2 * Sign) * AbsX;
}

#endif
