/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
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
