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

#ifndef __ARCHI_CHIPS_VIVOSOC3_FREQ_H__
#define __ARCHI_CHIPS_VIVOSOC3_FREQ_H__

// register address offsets
#define FREQ_CLK_TREE_CONF_GLOBAL_OFFSET     0x040 // base addr 0x1A104000 (ARCHI_APB_SOC_CTRL_ADDR)
#define FREQ_CLK_TREE_CONF_CL_OFFSET         0x050
#define FREQ_CLK_TREE_CONF_SOC_OFFSET        0x054
#define FREQ_CLK_TREE_CONF_PER_OFFSET        0x058
#define FREQ_CLK_TREE_CONF_PM_OFFSET         0x05C

#endif