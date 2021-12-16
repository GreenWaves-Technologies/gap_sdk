/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <math.h>
#include "ResizeBasicKernels.h"

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

void KerResizeBilinear(KerResizeBilinear_ArgT *Arg)

{
        unsigned char * __restrict__ In  = Arg->In;
        unsigned int Win                 = Arg->Win;
        unsigned int Hin                 = Arg->Hin;
        unsigned char * __restrict__ Out = Arg->Out;
        unsigned int Wout                = Arg->Wout;
        unsigned int Hout                = Arg->Hout;
        unsigned int HTileOut            = Arg->HTileOut;
        unsigned int FirstLineIndex      = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/Wout;
        unsigned int HStep = ((Hin-1)<<16)/Hout;

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = (hCoeff>>16);
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int offsetY = (hCoeff >> 16) - BaseY;
                unsigned int hc2 = (hCoeff >> 9) & 127;
                unsigned int hc1 = 128 - hc2;
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int offsetX = (wCoeff >> 16);
                        unsigned int wc2 = (wCoeff >> 9) & 127;
                        unsigned int wc1 = 128 - wc2;
                        unsigned int P1 = In[offsetY*Win       + offsetX    ];
                        unsigned int P2 = In[(offsetY + 1)*Win + offsetX    ];
                        unsigned int P3 = In[offsetY*Win       + offsetX + 1];
                        unsigned int P4 = In[(offsetY + 1)*Win + offsetX + 1];

                        Out[y*Wout + x] = ((P1*hc1 + P2*hc2)*wc1 + (P3*hc1 + P4*hc2)*wc2) >> 14;
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeNearestNeighbor(KerResizeNearestNeighbor_ArgT *Arg)

{
        unsigned char * __restrict__ In  = Arg->In;
        unsigned int Win                 = Arg->Win;
        unsigned int Hin                 = Arg->Hin;
        unsigned char * __restrict__ Out = Arg->Out;
        unsigned int Wout                = Arg->Wout;
        unsigned int Hout                = Arg->Hout;
        unsigned int HTileOut            = Arg->HTileOut;
        unsigned int FirstLineIndex      = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/(Wout-1);
        unsigned int HStep = ((Hin-1)<<16)/(Hout-1);

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = hCoeff;
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int h_rounded = gap_roundnormu(hCoeff - BaseY, 16);
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int w_rounded = gap_roundnormu(wCoeff, 16);

                        Out[y*Wout + x] = In[h_rounded*Win + w_rounded];
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeBilinearSigned(KerResizeBilinearSigned_ArgT *Arg)

{
        signed char * __restrict__ In  = Arg->In;
        unsigned int Win               = Arg->Win;
        unsigned int Hin               = Arg->Hin;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Wout              = Arg->Wout;
        unsigned int Hout              = Arg->Hout;
        unsigned int HTileOut          = Arg->HTileOut;
        unsigned int FirstLineIndex    = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/Wout;
        unsigned int HStep = ((Hin-1)<<16)/Hout;

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = (hCoeff>>16);
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int offsetY = (hCoeff >> 16) - BaseY;
                unsigned int hc2 = (hCoeff >> 9) & 127;
                unsigned int hc1 = 128 - hc2;
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int offsetX = (wCoeff >> 16);
                        unsigned int wc2 = (wCoeff >> 9) & 127;
                        unsigned int wc1 = 128 - wc2;
                        signed int P1 = In[offsetY*Win       + offsetX    ];
                        signed int P2 = In[(offsetY + 1)*Win + offsetX    ];
                        signed int P3 = In[offsetY*Win       + offsetX + 1];
                        signed int P4 = In[(offsetY + 1)*Win + offsetX + 1];

                        Out[y*Wout + x] = ((P1*hc1 + P2*hc2)*wc1 + (P3*hc1 + P4*hc2)*wc2) >> 14;
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeNearestNeighborSigned(KerResizeNearestNeighborSigned_ArgT *Arg)

{
        signed char   * __restrict__ In = Arg->In;
        unsigned int Win                = Arg->Win;
        unsigned int Hin                = Arg->Hin;
        signed char * __restrict__ Out  = Arg->Out;
        unsigned int Wout               = Arg->Wout;
        unsigned int Hout               = Arg->Hout;
        unsigned int HTileOut           = Arg->HTileOut;
        unsigned int FirstLineIndex     = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/(Wout-1);
        unsigned int HStep = ((Hin-1)<<16)/(Hout-1);

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = hCoeff;
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int h_rounded = gap_roundnormu(hCoeff - BaseY, 16);
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int w_rounded = gap_roundnormu(wCoeff, 16);

                        Out[y*Wout + x] = In[h_rounded*Win + w_rounded];
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeBilinearSigned_Q16(KerResizeSigned16_ArgT *Arg)

{
        signed short * __restrict__ In  = Arg->In;
        unsigned int Win                = Arg->Win;
        unsigned int Hin                = Arg->Hin;
        signed short * __restrict__ Out = Arg->Out;
        unsigned int Wout               = Arg->Wout;
        unsigned int Hout               = Arg->Hout;
        unsigned int HTileOut           = Arg->HTileOut;
        unsigned int FirstLineIndex     = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/Wout;
        unsigned int HStep = ((Hin-1)<<16)/Hout;

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = (hCoeff>>16);
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int offsetY = (hCoeff >> 16) - BaseY;
                unsigned int hc2 = (hCoeff >> 9) & 127;
                unsigned int hc1 = 128 - hc2;
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int offsetX = (wCoeff >> 16);
                        unsigned int wc2 = (wCoeff >> 9) & 127;
                        unsigned int wc1 = 128 - wc2;
                        signed int P1 = In[offsetY*Win       + offsetX    ];
                        signed int P2 = In[(offsetY + 1)*Win + offsetX    ];
                        signed int P3 = In[offsetY*Win       + offsetX + 1];
                        signed int P4 = In[(offsetY + 1)*Win + offsetX + 1];

                        Out[y*Wout + x] = ((P1*hc1 + P2*hc2)*wc1 + (P3*hc1 + P4*hc2)*wc2) >> 14;
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeNearestNeighborSigned_Q16(KerResizeSigned16_ArgT *Arg)

{
        signed short   * __restrict__ In = Arg->In;
        unsigned int Win                 = Arg->Win;
        unsigned int Hin                 = Arg->Hin;
        signed short * __restrict__ Out  = Arg->Out;
        unsigned int Wout                = Arg->Wout;
        unsigned int Hout                = Arg->Hout;
        unsigned int HTileOut            = Arg->HTileOut;
        unsigned int FirstLineIndex      = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/(Wout-1);
        unsigned int HStep = ((Hin-1)<<16)/(Hout-1);

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = hCoeff;
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int h_rounded = gap_roundnormu(hCoeff - BaseY, 16);
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int w_rounded = gap_roundnormu(wCoeff, 16);

                        Out[y*Wout + x] = In[h_rounded*Win + w_rounded];
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

#ifdef __gap9__
void KerResizeBilinear_fp16(KerResize_fp16_ArgT *Arg)

{
        F16 * __restrict__ In       = Arg->In;
        unsigned int Win            = Arg->Win;
        unsigned int Hin            = Arg->Hin;
        F16 * __restrict__ Out      = Arg->Out;
        unsigned int Wout           = Arg->Wout;
        unsigned int Hout           = Arg->Hout;
        unsigned int HTileOut       = Arg->HTileOut;
        unsigned int FirstLineIndex = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        F16 WStep = ((F16) (Win-1))/Wout;
        F16 HStep = ((F16) (Hin-1))/Hout;

        unsigned int x, y;
        F16 hCoeff = ((F16) HStep)*FirstLineIndex;
        F16 BaseY = hCoeff;
        for (y = 0 ; y < HTileOut ; y++) {
                int offsetY = (int) (hCoeff - BaseY);
                F16 hc2 = hCoeff;
                F16 hc1 = 1.0 - hc2;
                // unsigned int wCoeff = 0;
                F16 wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        int offsetX = (int) wCoeff;
                        F16 wc2 = wCoeff;
                        F16 wc1 = 1.0 - wc2;
                        F16 P1 = In[offsetY*Win       + offsetX    ];
                        F16 P2 = In[(offsetY + 1)*Win + offsetX    ];
                        F16 P3 = In[offsetY*Win       + offsetX + 1];
                        F16 P4 = In[(offsetY + 1)*Win + offsetX + 1];

                        Out[y*Wout + x] = ((P1*hc1 + P2*hc2)*wc1 + (P3*hc1 + P4*hc2)*wc2);
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}

void KerResizeNearestNeighbor_fp16(KerResize_fp16_ArgT *Arg)

{
        F16 * __restrict__ In       = Arg->In;
        unsigned int Win            = Arg->Win;
        unsigned int Hin            = Arg->Hin;
        F16 * __restrict__ Out      = Arg->Out;
        unsigned int Wout           = Arg->Wout;
        unsigned int Hout           = Arg->Hout;
        unsigned int HTileOut       = Arg->HTileOut;
        unsigned int FirstLineIndex = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        F16 WStep = ((F16) (Win-1))/(Wout-1);
        F16 HStep = ((F16) (Hin-1))/(Hout-1);

        unsigned int x, y;
        F16 hCoeff = HStep*FirstLineIndex;
        F16 BaseY = hCoeff;
        for (y = 0 ; y < HTileOut ; y++) {
                int h_rounded = (int) (hCoeff - BaseY + 0.5);
                F16 wCoeff = First*WStep;
                for (x = First ; x < Last ; x++) {
                        int w_rounded = (int) (wCoeff + 0.5);

                        Out[y*Wout + x] = In[h_rounded*Win + w_rounded];
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}
#endif
