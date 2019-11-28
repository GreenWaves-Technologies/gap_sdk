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

#ifndef DATA_TYPES_INC

#define DATA_TYPES_INC

typedef signed char    Pixs;
typedef unsigned char  Pixu;
typedef unsigned short Pixuw;
typedef signed   short Pixsw;
typedef unsigned char  PixuV __attribute__((vector_size (4)));
typedef unsigned short PixuVw __attribute__((vector_size (4)));
typedef signed char    PixsV __attribute__((vector_size (4)));
typedef signed short   PixsVw __attribute__((vector_size (4)));



typedef signed char   Filts;
typedef unsigned char FiltuV __attribute__((vector_size (4)));
typedef signed char   FiltsV __attribute__((vector_size (4)));

typedef signed short  WeightT;

#define MAX_LINE 4000

#endif
