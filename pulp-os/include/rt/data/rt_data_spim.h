/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __RT_DATA_RT_DATA_RT_DATA_SPIM_H__
#define __RT_DATA_RT_DATA_RT_DATA_SPIM_H__

typedef struct __rt_spim_s rt_spim_t;

typedef struct __rt_spim_s {
  int max_baudrate;
#if defined(UDMA_VERSION) && UDMA_VERSION >= 1
  unsigned int cfg;
  char cs;
#endif
  char wordsize;
  char big_endian;
  signed char cs_gpio;
  char channel;

#if !defined(UDMA_VERSION)

  int div;
  rt_spim_t *next;

#elif defined(UDMA_VERSION) && UDMA_VERSION >= 2

  char byte_align;
  unsigned char div;
  char polarity;
  char phase;

#elif defined(UDMA_VERSION) && UDMA_VERSION == 1

  unsigned char div;
  char polarity;
  char phase;

#endif

} rt_spim_t;

#endif
