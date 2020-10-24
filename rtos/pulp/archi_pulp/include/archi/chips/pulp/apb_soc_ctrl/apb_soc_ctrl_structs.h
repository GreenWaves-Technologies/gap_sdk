
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_APB_SOC_CTRL_STRUCTS__
#define __ARCHI_APB_SOC_CTRL_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int nb_cl           :16; // Number of clusters
    unsigned int nb_cores        :16; // Number of cores
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_ctrl_info_t;

typedef union {
  struct {
    unsigned int addr            :32; // FC Boot Address
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_ctrl_fc_boot_t;

typedef union {
  struct {
    unsigned int fc_fe           :1 ; // FC Fetch Enable
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_ctrl_fc_fetch_t;

typedef union {
  struct {
    unsigned int status          :32; // Chip status register. The SW can store the exit value value so that the external loader can get it.
    unsigned int eoc             :1 ; // End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished.
  };
  unsigned int raw;
} __attribute__((packed)) apb_soc_ctrl_corestatus_t;

#endif

#endif
