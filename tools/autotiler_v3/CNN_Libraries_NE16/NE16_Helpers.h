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

#ifndef __NE16_HELPERS_H__
#define __NE16_HELPERS_H__

#include "Gap.h"
#include "hal_ne16.h"

static inline void SetNE16_InPointer(void *InPointer)
{
        NE16_WRITE_REG(NE16_REG_INFEAT_PTR, (int)InPointer);
#if defined(DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
        printf("InPointer:\t%x\n", InPointer);
#endif
}
static inline void SetNE16_OutPointer(void *OutPointer)
{
        NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR, (int)OutPointer);
#if defined(DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
        printf("OutPointer:\t%x\n", OutPointer);
#endif
}
static inline void SetNE16_WeightsPointer(void *WeightsPointer)
{
        NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR, (int)WeightsPointer);
#ifdef DEBUG_NE16
        printf("WeightsPointer:\t%x\n", WeightsPointer);
#endif
}
static inline void SetNE16_BiasPointer(void *BiasPointer)
{
        NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR, (int)BiasPointer);
#ifdef DEBUG_NE16
        printf("BiasPointer:\t%x\n", BiasPointer);
#endif
}
static inline void SetNE16_ScalePointer(void *ScalePointer)
{
        NE16_WRITE_REG(NE16_REG_SCALE_PTR, (int)ScalePointer);
#ifdef DEBUG_NE16
        printf("ScalePointer:\t%x\n", ScalePointer);
#endif
}
static inline void SetNE16_ScaleNPointer(void *ScaleNPointer)
{
        NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, (int)ScaleNPointer);
#ifdef DEBUG_NE16
        printf("ScaleNPointer:\t%x\n", ScaleNPointer);
#endif
}

static inline void SetNE16_Strides(unsigned short In_D0,
                                   unsigned short In_D1,
                                   unsigned short In_D2,

                                   unsigned short Out_D0,
                                   unsigned short Out_D1,
                                   unsigned short Out_D2,

                                   unsigned short Weights_D0,
                                   unsigned short Weights_D1,
                                   unsigned short Weights_D2)
{
#ifdef DEBUG_NE16
        printf("InStrides: %d - %d - %d OutStrides: %d - %d - %d WeightsStrides: %d - %d - %d\n", In_D0, In_D1, In_D2, Out_D0, Out_D1, Out_D2, Weights_D0, Weights_D1, Weights_D2);
#endif
        NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE, In_D0);
        NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE, In_D1);
        NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE, In_D2);
        NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, Out_D0);
        NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, Out_D1);
        NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, Out_D2);
        NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, Weights_D0);
        NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, Weights_D1);
        NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, Weights_D2);
}

static inline void SetNE16_Dim(unsigned short Nb_KI,
                               unsigned short Nb_KO,
                               unsigned short Nb_WO,
                               unsigned short Nb_HO)
{
#ifdef DEBUG_NE16
        printf("Nb_KI:\t%d\tNb_KO:\t%d\tNb_HO:\t%d\tNb_WO:\t%d\n", Nb_KI, Nb_KO, Nb_WO, Nb_HO);
#endif
        NE16_WRITE_REG(NE16_REG_NB_KO_KI, ((Nb_KI & NE16_MASK_NB_KI) << NE16_SHIFT_NB_KI) |     /**< The Ki remainder must be the remainder of Ki / 16 if such remainder is not zero, otherwise must be 16 */
                                              ((Nb_KO & NE16_MASK_NB_KO) << NE16_SHIFT_NB_KO)); /**< The number of Ko subtiles is Ko / 32, plus 1 if Ko / 32 has non-zero remainder */
        NE16_WRITE_REG(NE16_REG_NB_HO_WO, ((Nb_WO & NE16_MASK_NB_WO) << NE16_SHIFT_NB_WO) |     /**< The number of Wo subtitles is Wo / 3, plus 1 if Wo / 3 has non-zero remainder */
                                              ((Nb_HO & NE16_MASK_NB_HO) << NE16_SHIFT_NB_HO)); /**< The number of Ho subtitles is Ho / 3, plus 1 if Ho / 3 has non-zero remainder */
}

static inline void SetNE16_Reminders(unsigned short Rem_WI,
                                     unsigned short Rem_HI,
                                     unsigned short Rem_KI,
                                     unsigned short Rem_KO,
                                     unsigned short Rem_WO,
                                     unsigned short Rem_HO)
{
#ifdef DEBUG_NE16
        printf("Rem_KI:\t%d\tRem_KO:\t%d\tRem_HI:\t%d\tRem_WI:\t%d\tRem_HO:\t%d\tRem_WO:\t%d\n", Rem_KI, Rem_KO, Rem_HI, Rem_WI, Rem_HO, Rem_WO);
#endif
        NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((Rem_KI & NE16_MASK_REM_KI) << NE16_SHIFT_REM_KI) |     /**< The Ki remainder must be the remainder of Ki / 16 if such remainder is not zero, otherwise must be 16 */
                                               ((Rem_KO & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO)); /**< The Ko remainder must be the remainder of Ko / 32 if such remainder is not zero, otherwise must be 32 */
        NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((Rem_WO & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) |     /**< The Wo remainder must be the remainder of Wo / 3 */
                                               ((Rem_HO & NE16_MASK_REM_HO) << NE16_SHIFT_REM_HO)); /**< The Ho remainder must be the remainder of Ho / 3 */
        NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((Rem_WI & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) |     /**< */
                                               ((Rem_HI & NE16_MASK_REM_HI) << NE16_SHIFT_REM_HI)); /**< */
}

static inline void SetNE16_ConfigPad(v4s Pad, short int PadVal)
{
        NE16_WRITE_REG(NE16_REG_PADDING, ((PadVal & NE16_MASK_PADDING_VALUE) << NE16_SHIFT_PADDING_VALUE) |     /**< Only if the NE16 is set to 3x3 mode. Explicit padding forces the value of part of the input set.  */
                                             ((Pad[0] & NE16_MASK_PADDING_LEFT) << NE16_SHIFT_PADDING_LEFT) |   /**< It can be from 0 to 2 in each direction (left/right/top/bottom) */
                                             ((Pad[1] & NE16_MASK_PADDING_RIGHT) << NE16_SHIFT_PADDING_RIGHT) | /**< The padding value can be from 0 to 255 in basic mode and from 0 to 65535 in mode16 */
                                             ((Pad[2] & NE16_MASK_PADDING_TOP) << NE16_SHIFT_PADDING_TOP) |
                                             ((Pad[3] & NE16_MASK_PADDING_BOTTOM) << NE16_SHIFT_PADDING_BOTTOM));
#ifdef DEBUG_NE16
        printf("Padding: {%d, %d, %d, %d}, Val: %d\n", Pad[0], Pad[1], Pad[2], Pad[3], PadVal);
#endif
}

static inline void SetNE16_ConfigFMask(v4s FilterMask)
{
        NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((FilterMask[0] & NE16_MASK_FILTER_MASK_LEFT) << NE16_SHIFT_FILTER_MASK_LEFT) |       /**< Only if the NE16 is set to 3x3 mode. */
                                                 ((FilterMask[1] & NE16_MASK_FILTER_MASK_RIGHT) << NE16_SHIFT_FILTER_MASK_RIGHT) | /**< Filter masking forces the value of part of the weights in the spatial direction */
                                                 ((FilterMask[2] & NE16_MASK_FILTER_MASK_TOP) << NE16_SHIFT_FILTER_MASK_TOP) |     /**< It can be from 0 to 1 in each direction (left/right/top/bottom) */
                                                 ((FilterMask[3] & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM));
#ifdef DEBUG_NE16
        printf("FMask: {%d, %d, %d, %d}\n", FilterMask[0], FilterMask[1], FilterMask[2], FilterMask[3]);
#endif
}

static inline void SetNE16_WOffset(int W_Offset)
{
        NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, W_Offset);
#ifdef DEBUG_NE16
        printf("W_Offset: %d\n", W_Offset);
#endif
}

static inline void PrintNE16_GenConfig(unsigned int Cfg)
{
#ifdef DEBUG_NE16
        int Qw = ((Cfg >> NE16_SHIFT_WBITS_M1) & NE16_MASK_WBITS_M1) + 1;
        int Mode16 = ((Cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16);
        int StreamoutMode = ((Cfg >> NE16_SHIFT_OUTQUANT) & NE16_MASK_OUTQUANT);
        int FilterMode = ((Cfg >> NE16_SHIFT_FILTER_MODE) & NE16_MASK_FILTER_MODE);
        int LinearMode = ((Cfg >> NE16_SHIFT_LINEAR_MODE) & NE16_MASK_LINEAR_MODE);
        int StridedMode = ((Cfg >> NE16_SHIFT_STRIDED_MODE) & NE16_MASK_STRIDED_MODE);
        int NormBits = ((Cfg >> NE16_SHIFT_NORM_BITS) & NE16_MASK_NORM_BITS);
        int Streamin = ((Cfg >> NE16_SHIFT_STREAMIN) & NE16_MASK_STREAMIN);
        int WOffsetCfg = ((Cfg >> NE16_SHIFT_WEIGHT_OFFSET_CFG) & NE16_MASK_WEIGHT_OFFSET_CFG);
        int QuantRightShift = ((Cfg >> NE16_SHIFT_QUANT_RIGHT_SHIFT) & NE16_MASK_QUANT_RIGHT_SHIFT);
        int QuantBits = ((Cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS);
        int QuantNoRect = ((Cfg >> NE16_SHIFT_QUANT_NORECT) & NE16_MASK_QUANT_NORECT);
        int NormShift = ((Cfg >> NE16_SHIFT_NORM_SHIFT) & NE16_MASK_NORM_SHIFT);
        int NormBias = ((Cfg >> NE16_SHIFT_NORM_BIAS) & NE16_MASK_NORM_BIAS);
        printf("General config: %d\n\tQw: %d Mode16: %d StreamoutMode: %d FilterMode: %d LinearMode: %d\n\tStridedMode: %d NormBits: %d Streamin: %d WOffsetCfg: %d\n\tQuantRightShift: %d QuantBits: %d QuantNoRect: %d NormShift: %d\n\tNormBias %d\n", Cfg, Qw, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);
#endif
}

static inline void SetNE16_GenConfig(unsigned int Cfg)
{
        NE16_WRITE_REG(NE16_REG_CONFIG, 0);
        NE16_WRITE_REG(NE16_REG_CONFIG, Cfg);
#ifdef DEBUG_NE16
        PrintNE16_GenConfig(Cfg);
#endif
}


static inline void CalcNumIn(int cfg, int Nin, int *Rem_KI, int *Nb_KI)
{
        // int KiTileBig = Nin / 256;
        // int KiTileBigRem = KiTileBig % 256;
        // int KiTileSmall = (KiTileBigRem?KiTileBigRem/16 : 0);
        // if (KiTileSmall % 16) printf("WARNING!!! - Nin must be divisible by 16");
        // *Rem_KI = KiTileSmall;
        // *Nb_KI = KiTileBig + (KiTileSmall ? 1 : 0);
        int divisor = ((cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16 ? 512 : 256);
        *Rem_KI = ((Nin % divisor) / 16) == 0 ? 16 : (Nin % divisor) / 16;
        *Nb_KI = Nin / divisor + (Nin % divisor ? 1 : 0);
}

static inline int GetJobId()
{
        volatile int job_id;
        // printf("cfg: %x\n", cfg);
        // acquire job
        NE16_BARRIER_ACQUIRE(job_id);
        return job_id;
}

static inline void SetupNE16Job(int Cfg, void *pIn, void *pOut, void *pWeights, void *pBias, int OutBytes, int NumIn, int NumOut, int NumTileOut, int RemOut, int NumWOut, int NumHOut, int Qw, void *pScale, void *pScaleN, int WOff)
{

        int Rem_KI = 0, Nb_KI = 0;
        CalcNumIn(Cfg, NumIn, &Rem_KI, &Nb_KI);
        // printf("Nin %d, Rem_KI %d, Nb_KI %d\n", DimIn, Rem_KI, Nb_KI);

        if (pBias)
        {
                Cfg |= (NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
        }
        else
        {
                Cfg &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
        }

        // load configuration for the layer - input only
        SetNE16_InPointer(pIn);
        SetNE16_OutPointer(pOut);
        SetNE16_WeightsPointer(pWeights);
        SetNE16_BiasPointer(pBias);
        SetNE16_ScalePointer(pScale);
        SetNE16_ScaleNPointer(pScaleN);
        SetNE16_Strides(16, 0, 0,                                           // In_D0, In_D1 - unused, In_D2 - unused
                        OutBytes * 8, OutBytes * NumOut, OutBytes * NumOut, // Out_D0, Out_D1 - unused, Out_D2 - unused
                        // OutBytes * 8, 0, 0,                                   // Out_D0, Out_D1 - unused, Out_D2 - unused
                        NumIn * 2 / 16, Qw * NumIn * 2 / 16, Qw * NumIn * 2); // Weights_D0, Weights_D1, Weights_D2
        SetNE16_Reminders(0, 0, Rem_KI, RemOut, 1, 1);
        SetNE16_Dim(Nb_KI, NumTileOut, 1, 1);
        SetNE16_WOffset(WOff);
        SetNE16_ConfigPad((v4s){0, 0, 0, 0}, 0);
        SetNE16_ConfigFMask((v4s){0, 0, 0, 0});
        SetNE16_GenConfig(Cfg);
}

static inline void TriggerNE16Job(int Cfg, void *pIn, void *pOut, void *pWeights, void *pBias, int OutBytes, int NumIn, int NumOut, int NumTileOut, int RemOut, int NumWOut, int NumHOut, int Qw, void *pScale, void *pScaleN, int WOff)
{
        GetJobId();
        SetupNE16Job(Cfg, pIn, pOut, pWeights, pBias, OutBytes, NumIn, NumOut, NumTileOut, RemOut, NumWOut, NumHOut, Qw, pScale, pScaleN, WOff);
        // commit and trigger NE16 computation
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
}

#endif
