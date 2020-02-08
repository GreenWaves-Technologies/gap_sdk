/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __ARCHI_GVSOC_GVSOC_H__
#define __ARCHI_GVSOC_GVSOC_H__

#define GV_SEMIHOSTING_FIRST_CUSTOM             0x1000

#define GV_SEMIHOSTING_VCD_CONFIGURE            0x1000
#define GV_SEMIHOSTING_VCD_OPEN_TRACE           0x1001
#define GV_SEMIHOSTING_VCD_CONF_TRACE           0x1002
#define GV_SEMIHOSTING_VCD_DUMP_TRACE           0x1003
#define GV_SEMIHOSTING_VCD_DUMP_TRACE_STRING    0x1004

#define GV_SEMIHOSTING_TRACE_OPEN               0x1010
#define GV_SEMIHOSTING_TRACE_ENABLE             0x1011

#endif
