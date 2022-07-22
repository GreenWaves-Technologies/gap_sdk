
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

#ifndef __ARCHI_GPIO_REGFIELD__
#define __ARCHI_GPIO_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
#define GPIO_PADDIR_00_31_PADDIR_BIT                                 0
#define GPIO_PADDIR_00_31_PADDIR_WIDTH                               32
#define GPIO_PADDIR_00_31_PADDIR_MASK                                0xffffffff
#define GPIO_PADDIR_00_31_PADDIR_RESET                               0x0

// GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
#define GPIO_GPIOEN_00_31_GPIOEN_BIT                                 0
#define GPIO_GPIOEN_00_31_GPIOEN_WIDTH                               32
#define GPIO_GPIOEN_00_31_GPIOEN_MASK                                0xffffffff
#define GPIO_GPIOEN_00_31_GPIOEN_RESET                               0x0

// GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
#define GPIO_PADIN_00_31_PADIN_BIT                                   0
#define GPIO_PADIN_00_31_PADIN_WIDTH                                 32
#define GPIO_PADIN_00_31_PADIN_MASK                                  0xffffffff
#define GPIO_PADIN_00_31_PADIN_RESET                                 0x0

// GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
#define GPIO_PADOUT_00_31_PADOUT_BIT                                 0
#define GPIO_PADOUT_00_31_PADOUT_WIDTH                               32
#define GPIO_PADOUT_00_31_PADOUT_MASK                                0xffffffff
#define GPIO_PADOUT_00_31_PADOUT_RESET                               0x0

// GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
#define GPIO_PADOUTSET_00_31_PADOUTSET_BIT                           0
#define GPIO_PADOUTSET_00_31_PADOUTSET_WIDTH                         32
#define GPIO_PADOUTSET_00_31_PADOUTSET_MASK                          0xffffffff
#define GPIO_PADOUTSET_00_31_PADOUTSET_RESET                         0x0

// GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_BIT                           0
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_WIDTH                         32
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_MASK                          0xffffffff
#define GPIO_PADOUTCLR_00_31_PADOUTCLR_RESET                         0x0

// GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
#define GPIO_INTEN_00_31_INTEN_BIT                                   0
#define GPIO_INTEN_00_31_INTEN_WIDTH                                 32
#define GPIO_INTEN_00_31_INTEN_MASK                                  0xffffffff
#define GPIO_INTEN_00_31_INTEN_RESET                                 0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_00_15_INTTYPE_BIT                               0
#define GPIO_INTTYPE_00_15_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_00_15_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_00_15_INTTYPE_RESET                             0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_16_31_INTTYPE_BIT                               0
#define GPIO_INTTYPE_16_31_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_16_31_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_16_31_INTTYPE_RESET                             0x0

// GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
#define GPIO_INTSTATUS_00_31_INTSTATUS_BIT                           0
#define GPIO_INTSTATUS_00_31_INTSTATUS_WIDTH                         32
#define GPIO_INTSTATUS_00_31_INTSTATUS_MASK                          0xffffffff
#define GPIO_INTSTATUS_00_31_INTSTATUS_RESET                         0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_00_03_PADCFG_BIT                                 0
#define GPIO_PADCFG_00_03_PADCFG_WIDTH                               4
#define GPIO_PADCFG_00_03_PADCFG_MASK                                0xf
#define GPIO_PADCFG_00_03_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_04_07_PADCFG_BIT                                 0
#define GPIO_PADCFG_04_07_PADCFG_WIDTH                               4
#define GPIO_PADCFG_04_07_PADCFG_MASK                                0xf
#define GPIO_PADCFG_04_07_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_08_11_PADCFG_BIT                                 0
#define GPIO_PADCFG_08_11_PADCFG_WIDTH                               4
#define GPIO_PADCFG_08_11_PADCFG_MASK                                0xf
#define GPIO_PADCFG_08_11_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_12_15_PADCFG_BIT                                 0
#define GPIO_PADCFG_12_15_PADCFG_WIDTH                               4
#define GPIO_PADCFG_12_15_PADCFG_MASK                                0xf
#define GPIO_PADCFG_12_15_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_16_19_PADCFG_BIT                                 0
#define GPIO_PADCFG_16_19_PADCFG_WIDTH                               4
#define GPIO_PADCFG_16_19_PADCFG_MASK                                0xf
#define GPIO_PADCFG_16_19_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_20_23_PADCFG_BIT                                 0
#define GPIO_PADCFG_20_23_PADCFG_WIDTH                               4
#define GPIO_PADCFG_20_23_PADCFG_MASK                                0xf
#define GPIO_PADCFG_20_23_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_24_27_PADCFG_BIT                                 0
#define GPIO_PADCFG_24_27_PADCFG_WIDTH                               4
#define GPIO_PADCFG_24_27_PADCFG_MASK                                0xf
#define GPIO_PADCFG_24_27_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_28_31_PADCFG_BIT                                 0
#define GPIO_PADCFG_28_31_PADCFG_WIDTH                               4
#define GPIO_PADCFG_28_31_PADCFG_MASK                                0xf
#define GPIO_PADCFG_28_31_PADCFG_RESET                               0x0

// GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
#define GPIO_PADDIR_32_63_PADDIR_BIT                                 0
#define GPIO_PADDIR_32_63_PADDIR_WIDTH                               32
#define GPIO_PADDIR_32_63_PADDIR_MASK                                0xffffffff
#define GPIO_PADDIR_32_63_PADDIR_RESET                               0x0

// GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
#define GPIO_GPIOEN_32_63_GPIOEN_BIT                                 0
#define GPIO_GPIOEN_32_63_GPIOEN_WIDTH                               32
#define GPIO_GPIOEN_32_63_GPIOEN_MASK                                0xffffffff
#define GPIO_GPIOEN_32_63_GPIOEN_RESET                               0x0

// GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
#define GPIO_PADIN_32_63_PADIN_BIT                                   0
#define GPIO_PADIN_32_63_PADIN_WIDTH                                 32
#define GPIO_PADIN_32_63_PADIN_MASK                                  0xffffffff
#define GPIO_PADIN_32_63_PADIN_RESET                                 0x0

// GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
#define GPIO_PADOUT_32_63_PADOUT_BIT                                 0
#define GPIO_PADOUT_32_63_PADOUT_WIDTH                               32
#define GPIO_PADOUT_32_63_PADOUT_MASK                                0xffffffff
#define GPIO_PADOUT_32_63_PADOUT_RESET                               0x0

// GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
#define GPIO_PADOUTSET_32_63_PADOUTSET_BIT                           0
#define GPIO_PADOUTSET_32_63_PADOUTSET_WIDTH                         32
#define GPIO_PADOUTSET_32_63_PADOUTSET_MASK                          0xffffffff
#define GPIO_PADOUTSET_32_63_PADOUTSET_RESET                         0x0

// GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_BIT                           0
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_WIDTH                         32
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_MASK                          0xffffffff
#define GPIO_PADOUTCLR_32_63_PADOUTCLR_RESET                         0x0

// GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
#define GPIO_INTEN_32_63_INTEN_BIT                                   0
#define GPIO_INTEN_32_63_INTEN_WIDTH                                 32
#define GPIO_INTEN_32_63_INTEN_MASK                                  0xffffffff
#define GPIO_INTEN_32_63_INTEN_RESET                                 0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_32_47_INTTYPE_BIT                               0
#define GPIO_INTTYPE_32_47_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_32_47_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_32_47_INTTYPE_RESET                             0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_48_63_INTTYPE_BIT                               0
#define GPIO_INTTYPE_48_63_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_48_63_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_48_63_INTTYPE_RESET                             0x0

// GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
#define GPIO_INTSTATUS_32_63_INTSTATUS_BIT                           0
#define GPIO_INTSTATUS_32_63_INTSTATUS_WIDTH                         32
#define GPIO_INTSTATUS_32_63_INTSTATUS_MASK                          0xffffffff
#define GPIO_INTSTATUS_32_63_INTSTATUS_RESET                         0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_32_35_PADCFG_BIT                                 0
#define GPIO_PADCFG_32_35_PADCFG_WIDTH                               4
#define GPIO_PADCFG_32_35_PADCFG_MASK                                0xf
#define GPIO_PADCFG_32_35_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_36_39_PADCFG_BIT                                 0
#define GPIO_PADCFG_36_39_PADCFG_WIDTH                               4
#define GPIO_PADCFG_36_39_PADCFG_MASK                                0xf
#define GPIO_PADCFG_36_39_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_40_43_PADCFG_BIT                                 0
#define GPIO_PADCFG_40_43_PADCFG_WIDTH                               4
#define GPIO_PADCFG_40_43_PADCFG_MASK                                0xf
#define GPIO_PADCFG_40_43_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_44_47_PADCFG_BIT                                 0
#define GPIO_PADCFG_44_47_PADCFG_WIDTH                               4
#define GPIO_PADCFG_44_47_PADCFG_MASK                                0xf
#define GPIO_PADCFG_44_47_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_48_51_PADCFG_BIT                                 0
#define GPIO_PADCFG_48_51_PADCFG_WIDTH                               4
#define GPIO_PADCFG_48_51_PADCFG_MASK                                0xf
#define GPIO_PADCFG_48_51_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_52_55_PADCFG_BIT                                 0
#define GPIO_PADCFG_52_55_PADCFG_WIDTH                               4
#define GPIO_PADCFG_52_55_PADCFG_MASK                                0xf
#define GPIO_PADCFG_52_55_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_56_59_PADCFG_BIT                                 0
#define GPIO_PADCFG_56_59_PADCFG_WIDTH                               4
#define GPIO_PADCFG_56_59_PADCFG_MASK                                0xf
#define GPIO_PADCFG_56_59_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_60_63_PADCFG_BIT                                 0
#define GPIO_PADCFG_60_63_PADCFG_WIDTH                               4
#define GPIO_PADCFG_60_63_PADCFG_MASK                                0xf
#define GPIO_PADCFG_60_63_PADCFG_RESET                               0x0

// GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
#define GPIO_PADDIR_64_95_PADDIR_BIT                                 0
#define GPIO_PADDIR_64_95_PADDIR_WIDTH                               32
#define GPIO_PADDIR_64_95_PADDIR_MASK                                0xffffffff
#define GPIO_PADDIR_64_95_PADDIR_RESET                               0x0

// GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
#define GPIO_GPIOEN_64_95_GPIOEN_BIT                                 0
#define GPIO_GPIOEN_64_95_GPIOEN_WIDTH                               32
#define GPIO_GPIOEN_64_95_GPIOEN_MASK                                0xffffffff
#define GPIO_GPIOEN_64_95_GPIOEN_RESET                               0x0

// GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
#define GPIO_PADIN_64_95_PADIN_BIT                                   0
#define GPIO_PADIN_64_95_PADIN_WIDTH                                 32
#define GPIO_PADIN_64_95_PADIN_MASK                                  0xffffffff
#define GPIO_PADIN_64_95_PADIN_RESET                                 0x0

// GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
#define GPIO_PADOUT_64_95_PADOUT_BIT                                 0
#define GPIO_PADOUT_64_95_PADOUT_WIDTH                               32
#define GPIO_PADOUT_64_95_PADOUT_MASK                                0xffffffff
#define GPIO_PADOUT_64_95_PADOUT_RESET                               0x0

// GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
#define GPIO_PADOUTSET_64_95_PADOUTSET_BIT                           0
#define GPIO_PADOUTSET_64_95_PADOUTSET_WIDTH                         32
#define GPIO_PADOUTSET_64_95_PADOUTSET_MASK                          0xffffffff
#define GPIO_PADOUTSET_64_95_PADOUTSET_RESET                         0x0

// GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
#define GPIO_PADOUTCLR_64_95_PADOUTCLR_BIT                           0
#define GPIO_PADOUTCLR_64_95_PADOUTCLR_WIDTH                         32
#define GPIO_PADOUTCLR_64_95_PADOUTCLR_MASK                          0xffffffff
#define GPIO_PADOUTCLR_64_95_PADOUTCLR_RESET                         0x0

// GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
#define GPIO_INTEN_64_95_INTEN_BIT                                   0
#define GPIO_INTEN_64_95_INTEN_WIDTH                                 32
#define GPIO_INTEN_64_95_INTEN_MASK                                  0xffffffff
#define GPIO_INTEN_64_95_INTEN_RESET                                 0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_64_79_INTTYPE_BIT                               0
#define GPIO_INTTYPE_64_79_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_64_79_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_64_79_INTTYPE_RESET                             0x0

// GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
#define GPIO_INTTYPE_80_95_INTTYPE_BIT                               0
#define GPIO_INTTYPE_80_95_INTTYPE_WIDTH                             32
#define GPIO_INTTYPE_80_95_INTTYPE_MASK                              0xffffffff
#define GPIO_INTTYPE_80_95_INTTYPE_RESET                             0x0

// GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
#define GPIO_INTSTATUS_64_95_INTSTATUS_BIT                           0
#define GPIO_INTSTATUS_64_95_INTSTATUS_WIDTH                         32
#define GPIO_INTSTATUS_64_95_INTSTATUS_MASK                          0xffffffff
#define GPIO_INTSTATUS_64_95_INTSTATUS_RESET                         0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_64_67_PADCFG_BIT                                 0
#define GPIO_PADCFG_64_67_PADCFG_WIDTH                               4
#define GPIO_PADCFG_64_67_PADCFG_MASK                                0xf
#define GPIO_PADCFG_64_67_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_68_71_PADCFG_BIT                                 0
#define GPIO_PADCFG_68_71_PADCFG_WIDTH                               4
#define GPIO_PADCFG_68_71_PADCFG_MASK                                0xf
#define GPIO_PADCFG_68_71_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_72_75_PADCFG_BIT                                 0
#define GPIO_PADCFG_72_75_PADCFG_WIDTH                               4
#define GPIO_PADCFG_72_75_PADCFG_MASK                                0xf
#define GPIO_PADCFG_72_75_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_76_79_PADCFG_BIT                                 0
#define GPIO_PADCFG_76_79_PADCFG_WIDTH                               4
#define GPIO_PADCFG_76_79_PADCFG_MASK                                0xf
#define GPIO_PADCFG_76_79_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_80_83_PADCFG_BIT                                 0
#define GPIO_PADCFG_80_83_PADCFG_WIDTH                               4
#define GPIO_PADCFG_80_83_PADCFG_MASK                                0xf
#define GPIO_PADCFG_80_83_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_84_87_PADCFG_BIT                                 0
#define GPIO_PADCFG_84_87_PADCFG_WIDTH                               4
#define GPIO_PADCFG_84_87_PADCFG_MASK                                0xf
#define GPIO_PADCFG_84_87_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_88_91_PADCFG_BIT                                 0
#define GPIO_PADCFG_88_91_PADCFG_WIDTH                               4
#define GPIO_PADCFG_88_91_PADCFG_MASK                                0xf
#define GPIO_PADCFG_88_91_PADCFG_RESET                               0x0

// GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
#define GPIO_PADCFG_92_95_PADCFG_BIT                                 0
#define GPIO_PADCFG_92_95_PADCFG_WIDTH                               4
#define GPIO_PADCFG_92_95_PADCFG_MASK                                0xf
#define GPIO_PADCFG_92_95_PADCFG_RESET                               0x0

#endif
