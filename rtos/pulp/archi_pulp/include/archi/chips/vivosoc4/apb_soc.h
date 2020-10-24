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

#ifndef __ARCHI_VIVOSOC3_APB_SOC_H__
#define __ARCHI_VIVOSOC3_APB_SOC_H__
 
#define APB_SOC_BOOT_OTHER    0
#define APB_SOC_BOOT_JTAG     1
#define APB_SOC_BOOT_SPI      2
#define APB_SOC_BOOT_ROM      3
#define APB_SOC_BOOT_PRELOAD  4
#define APB_SOC_BOOT_HYPER    5
#define APB_SOC_BOOT_SPIM     6
#define APB_SOC_BOOT_SPIM_QPI 7


#define APB_SOC_INFO_OFFSET             0x00 // contains number of cores [31:16] and clusters [15:0]
#define APB_SOC_INFOEXTD_OFFSET         0x04 // contains chip name in ASCII
#define APB_SOC_SPECPADS_OFFSET         0x08 // allows to sample and read pads of special and jtag group
#define APB_SOC_NOTUSED1_OFFSET         0x0C // not used at the moment

#define APB_SOC_BOOTADDR_OFFSET         0x10 // contains boot address of fabric controller
#define APB_SOC_FC_FE_OFFSET            0x14 // manual fetch enable for fc
#define APB_SOC_ANA_EVT_OFFSET          0x18 // enables sampling of analog event lines
#define APB_SOC_ANA_RSTN_OFFSET         0x1C // controls reset lines to analog IPs

#define APB_SOC_CLKDIV0_OFFSET          0x20 // cluster clock divider
#define APB_SOC_CLKDIV1_OFFSET          0x24 // soc clock divider
#define APB_SOC_CLKDIV2_OFFSET          0x28 // periph clock divider
#define APB_SOC_CLKDIV3_OFFSET          0x2C // pm clock divider

#define APB_SOC_CLK_ANA_TS_OFFSET       0x3C // controls clock enable and source for ana timesync

#define APB_SOC_CLKMUX_GLOBAL_OFFSET    0x40 // controls all clk muxes, divided into 4 groups for cluster, soc, per, pm
#define APB_SOC_FLL_CTRL_OFFSET         0x44 // controls enable and event generation for all FLLs, automatic duty cycling for SoC FLL
#define APB_SOC_FLL_RSTN_OFFSET         0x48 // allows manual reset of each FLL, 0 resets FLLs, automatically set back to 1 in next cycle

#define APB_SOC_CLKMUX_CLUSTER_OFFSET   0x50 // controls cluster clock muxes only
#define APB_SOC_CLKMUX_SOC_OFFSET       0x54 // controls soc clock muxes only
#define APB_SOC_CLKMUX_PER_OFFSET       0x58 // controls periph clock muxes only
#define APB_SOC_CLKMUX_PM_OFFSET        0x5C // controls pm clock muxes only
 
#define APB_SOC_PWRCMD_OFFSET           0x60 // change power modes
#define APB_SOC_PWRCFG_OFFSET           0x64 // configures power modes
#define APB_SOC_PWRREG_OFFSET           0x68 // 32 bit GP register used by power pngmt routines to see if is hard or cold reboot
#define APB_SOC_L2_PWRCTRL_OFFSET       0x6C // controls power enable to shared L2 slices

#define APB_SOC_WD_FLLSOC_OFFSET        0x70 // controls the watchdog for the clk_good of the soc FLL
#define APB_SOC_ANA_TS_CNT_OFFSET       0x74 // enables the counter for the analog timesync
#define APB_SOC_ANA_TS_PM_OFFSET        0x78 // sets the value for which a pm event is generated when matching the ana ts counter


#define APB_SOC_CORESTATUS_OFFSET       0x80 // 32bit GP register to be used during testing to return EOC(bit[31]) and status(bit[30:0])
#define APB_SOC_WD_MAIN_CTRL_OFFSET     0x84 // control register for the system watchdog
#define APB_SOC_WD_MAIN_DEARM_OFFSET    0x88 // writing this register resets the main watchdog counter

// aliases
#define APB_SOC_CLKDIV_CLUSTER_OFFSET   APB_SOC_CLKDIV0_OFFSET
#define APB_SOC_CLKDIV_SOC_OFFSET       APB_SOC_CLKDIV1_OFFSET
#define APB_SOC_CLKDIV_PERIPH_OFFSET    APB_SOC_CLKDIV2_OFFSET
#define APB_SOC_CLKDIV_PM_OFFSET        APB_SOC_CLKDIV3_OFFSET

#define APB_SOC_INFO_CORES_OFFSET       (APB_SOC_INFO_OFFSET + 2)
#define APB_SOC_INFO_CLUSTERS_OFFSET    (APB_SOC_INFO_OFFSET)

#define APB_SOC_STATUS_EOC_BIT          31
#define APB_SOC_NB_CORE_BIT             16

#define APB_SOC_FLL_CTRL_SOC_BIT        0
#define APB_SOC_FLL_CTRL_CLUSTER_BIT    1
#define APB_SOC_FLL_CTRL_PERIPH_BIT     2


#define APB_SOC_PADFUN0_OFFSET      0x100
#define APB_SOC_PADCFG0_OFFSET      0x180

#define APB_SOC_PADFUN_OFFSET(g)    (APB_SOC_PADFUN0_OFFSET+(g)*4) //sets the mux for pins  g*16+0 (bits [1:0]) to g*16+15 (bits [31:30])
#define APB_SOC_PADFUN_NO(pad)      ((pad) >> 4)
#define APB_SOC_PADFUN_PAD(padfun)  ((padfun)*16)
#define APB_SOC_PADFUN_SIZE         2
#define ARCHI_APB_SOC_PADFUN_NB     2
#define APB_SOC_PADFUN_BIT(pad)     (((pad) & 0xF) << 1)

// PADs configuration is made of 8bits out of which only the first 3 are used
// bit0    enable pull UP
// bit1    enable pull DOWN
// bit2    enable ST
// bit3..7 not used

#define APB_SOC_PADCFG_OFFSET(g)    (APB_SOC_PADCFG0_OFFSET+(g)*4) //sets config for pin  g*4+0(bits [7:0]) to pin  g*4+3(bits [31:24])
#define APB_SOC_PADCFG_NO(pad)      ((pad) >> 2)
#define APB_SOC_PADCFG_PAD(padfun)  ((padfun)*4)
#define APB_SOC_PADCFG_SIZE         8
#define APB_SOC_PADCFG_BIT(pad)     (((pad) & 0x3) << 3)

// aliases
#define APB_SOC_PADS_CONFIG         APB_SOC_PADCFG0_OFFSET

// HAL compatibility definitions, functions not supported
#define APB_SOC_PADS_CONFIG_BOOTSEL_BIT 0
#define APB_SOC_JTAG_REG_EXT_BIT        8
#define APB_SOC_JTAG_REG_EXT_WIDTH      4

#define APB_SOC_JTAG_REG_LOC_BIT        0
#define APB_SOC_JTAG_REG_LOC_WIDTH      4

#define APB_SOC_CLUSTER_ISOLATE_OFFSET  0x0C
#define APB_SOC_BUSY_OFFSET             0x6C
#define APB_SOC_JTAG_REG                0x74

#endif
