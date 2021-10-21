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

#ifndef __SSD_GENERATORS_FP16_H__
#define __SSD_GENERATORS_FP16_H__

/** @addtogroup groupCNN
@{ */

/**
 @brief Load Convolutional Neural Network Library

 Load Convolutional Neural Network Library
*/
extern void LoadSSDLibrary_fp16();

extern int CNN_SSD_PostProcess_fp16(
        char *Name,
        CNN_GenControl_T *Ctrl,
        int n_anchors,
        int n_classes,
        int n_outbox,
        int max_bb_before_nn_max,
        float DecScoreThr,
        float NMSThr);

#endif //__SSD_GENERATORS_FP16_H__
