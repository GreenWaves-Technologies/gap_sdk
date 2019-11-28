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

#ifndef SETUP_H
#define SETUP_H

//Cascade stride
//TODO not working with 2 check it out
#define DETECT_STRIDE 1
#define MAX_NUM_OUT_WINS 20
#define NON_MAX_THRES 250

#define CASCADE_STAGES_L1 5
#define CASCADE_TOTAL_STAGES 25

#define PERF_COUNT
//#define PRINT_FACE_ONLY

#define ENABLE_LAYER_1
#define ENABLE_LAYER_2
#define ENABLE_LAYER_3

#endif //SETUP_H