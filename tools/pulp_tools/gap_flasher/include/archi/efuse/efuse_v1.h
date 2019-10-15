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

#ifndef __ARCHI_EFUSE_EFUSE_V1_H__
#define __ARCHI_EFUSE_EFUSE_V1_H__

#define EFUSE_CMD_OFFSET  0x000
#define EFUSE_CFG_OFFSET  0x004
#define EFUSE_REGS_OFFSET 0x200

#define EFUSE_CMD_READ    0x1
#define EFUSE_CMD_WRITE   0x2
#define EFUSE_CMD_SLEEP   0x4

#endif