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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cable.hpp"


#define JTAG_SOC_INSTR_WIDTH 0x4
#define JTAG_SOC_IDCODE 0x2
#define JTAG_SOC_AXIREG 0x4
#define JTAG_SOC_BBMUXREG 0x5
#define JTAG_SOC_CLKGATEREG 0x6
#define JTAG_SOC_CONFREG 0x7
#define JTAG_SOC_TESTMODEREG 0x8
#define JTAG_SOC_BISTREG 0x9
#define JTAG_SOC_BYPASS 0xf
#define JTAG_SOC_IDCODE_WIDTH 32
#define JTAG_SOC_AXIREG_WIDTH 96
#define JTAG_SOC_BBMUXREG_WIDTH 21
#define JTAG_SOC_CLKGATEREG_WIDTH 11
#define JTAG_SOC_CONFREG_WIDTH 4
#define JTAG_SOC_TESTMODEREG_WIDTH 4
#define JTAG_SOC_BISTREG_WIDTH 20

#define JTAG_CLUSTER_INSTR_WIDTH 4
#define JTAG_CLUSTER_IDCODE 2
#define JTAG_CLUSTER_SAMPLE_PRELOAD 1
#define JTAG_CLUSTER_EXTEST 0
#define JTAG_CLUSTER_DEBUG 8
#define JTAG_CLUSTER_MBIST 9
#define JTAG_CLUSTER_BYPASS 0xf
#define JTAG_CLUSTER_IDCODE_WIDTH 32

#define JTAG_IDCODE_WIDTH JTAG_CLUSTER_IDCODE_WIDTH + JTAG_SOC_IDCODE_WIDTH
#define JTAG_INSTR_WIDTH JTAG_CLUSTER_INSTR_WIDTH + JTAG_SOC_INSTR_WIDTH


bool Cable_jtag_itf::jtag_soft_reset() {
  for (int i = 0; i < 10; i++)
    jtag_write_tms(1);

  jtag_write_tms(0);

  return true;
}

bool Cable_jtag_itf::jtag_write_tms(int val)
{
  return bit_inout(NULL, 0x0, val != 0);
}

bool Cable_jtag_itf::jtag_shift_ir()
{
  if (!jtag_write_tms(1)) return false;
  if (!jtag_write_tms(1)) return false;
  if (!jtag_write_tms(0)) return false;
  if (!jtag_write_tms(0)) return false;
  return true;
}

bool Cable_jtag_itf::jtag_shift_dr()
{
  if (!jtag_write_tms(1)) return false;
  if (!jtag_write_tms(0)) return false;
  if (!jtag_write_tms(0)) return false;
  return true;
}

bool Cable_jtag_itf::jtag_idle()
{
  if (!jtag_write_tms(1)) return false;
  if (!jtag_write_tms(0)) return false;
  return true;
}

bool Cable_jtag_itf::jtag_shift(int width, char *bits)
{
  return stream_inout(NULL, bits, width, 1);  
}

bool Cable_jtag_itf::jtag_shift_ir(unsigned int ir)
{
  if (!jtag_shift_ir()) return false;
  if (!jtag_shift(JTAG_SOC_INSTR_WIDTH, (char *)&ir)) return false;
  if (!jtag_idle()) return false;
  return true;
}

bool Cable_jtag_itf::jtag_set_reg(unsigned int reg, int width, unsigned int value)
{
  if (!jtag_shift_ir(reg)) return false;
  if (!jtag_shift_dr()) return false;
  if (!jtag_shift(width, (char *)&value)) return false;
  if (!jtag_idle()) return false;
  return true;
}

bool Cable_jtag_itf::jtag_get_reg(unsigned int reg, int width, unsigned int *out_value, unsigned int value)
{
  if (!jtag_shift_ir(reg)) return false;
  if (!jtag_shift_dr()) return false;
  if (!stream_inout((char *)out_value, (char *)&value, width, 1)) return false;
  if (!jtag_idle()) return false;
  return true;
}
