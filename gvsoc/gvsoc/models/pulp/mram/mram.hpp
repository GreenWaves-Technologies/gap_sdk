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


#include <vp/vp.hpp>

#define MRAM_CMD_TRIM_CFG    0x01
#define MRAM_CMD_NORMAL_TX   0x02
#define MRAM_CMD_ERASE_CHIP  0x04
#define MRAM_CMD_ERASE_SECT  0x08
#define MRAM_CMD_ERASE_WORD  0x10
#define MRAM_CMD_PWDN        0x20
#define MRAM_CMD_READ_RX     0x40
#define MRAM_CMD_REF_LINE_P  0x80
#define MRAM_CMD_REF_LINE_AP 0xC0

class Mram_itf 
{
public:
  void set_cmd(int cmd) { this->cmd = cmd; }

protected:
  int cmd;

};
