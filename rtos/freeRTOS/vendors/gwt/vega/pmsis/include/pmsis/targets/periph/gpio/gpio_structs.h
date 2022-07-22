
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

#ifndef __ARCHI_GPIO_STRUCTS__
#define __ARCHI_GPIO_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_paddir_00_31_t;

typedef union {
  struct {
    unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
  };
  unsigned int raw;
} __attribute__((packed)) gpio_gpioen_00_31_t;

typedef union {
  struct {
    unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padin_00_31_t;

typedef union {
  struct {
    unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padout_00_31_t;

typedef union {
  struct {
    unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padoutset_00_31_t;

typedef union {
  struct {
    unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padoutclr_00_31_t;

typedef union {
  struct {
    unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inten_00_31_t;

typedef union {
  struct {
    unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inttype_00_15_t;

typedef union {
  struct {
    unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inttype_16_31_t;

typedef union {
  struct {
    unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
  };
  unsigned int raw;
} __attribute__((packed)) gpio_intstatus_00_31_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_00_03_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_04_07_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_08_11_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_12_15_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_16_19_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_20_23_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_24_27_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_28_31_t;

typedef union {
  struct {
    unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_paddir_32_63_t;

typedef union {
  struct {
    unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
  };
  unsigned int raw;
} __attribute__((packed)) gpio_gpioen_32_63_t;

typedef union {
  struct {
    unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padin_32_63_t;

typedef union {
  struct {
    unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padout_32_63_t;

typedef union {
  struct {
    unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padoutset_32_63_t;

typedef union {
  struct {
    unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padoutclr_32_63_t;

typedef union {
  struct {
    unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inten_32_63_t;

typedef union {
  struct {
    unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inttype_32_47_t;

typedef union {
  struct {
    unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
  };
  unsigned int raw;
} __attribute__((packed)) gpio_inttype_48_63_t;

typedef union {
  struct {
    unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
  };
  unsigned int raw;
} __attribute__((packed)) gpio_intstatus_32_63_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_32_35_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_36_39_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_40_43_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_44_47_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_48_51_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_52_55_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_56_59_t;

typedef union {
  struct {
    unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
  };
  unsigned int raw;
} __attribute__((packed)) gpio_padcfg_60_63_t;

#endif

#endif
