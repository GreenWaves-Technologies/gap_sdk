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

#ifndef __ARCHI_RTC_H__
#define __ARCHI_RTC_H__

/*
 *  The RTC should be used through the APB interface. In this header file, there are:
 *      -   APB registers which are direct access registers, with ADDR: ( SOC_PERIPHERALS_BASE_ADDR + 0x7000 + APB_RTC_**_OFFSET)
 *      -   RTC registers which are indirect access registers. These regs should be R/W via the APB registers.
 *
 *  For distinguishing these two types of registers:
 *      -   APB registers:  All the defines are named in UPPER_CASE
 *      -   RTC registers:  All the difines are named in Initial_Capitalization
 *
 *  ARCHI_RTC_ADDR = ( SOC_PERIPHERALS_BASE_ADDR + 0x8000 )
 */


#define RTC_RTC_INT_EVENT       ARCHI_SOC_EVENT_RTC_IRQ
#define RTC_RTC_APB_EVENT       ARCHI_SOC_EVENT_RTC_APB_IRQ

//  Address of APB registers
#define APB_RTC_STAT_OFFSET                 0x00
#define APB_RTC_CTRL_OFFSET                 0x04
#define APB_RTC_DATA_OFFSET                 0x08
#define APB_RTC_IRQ_CTRL_OFFSET             0x10
#define APB_RTC_IRQ_MASK_OFFSET             0x14
#define APB_RTC_IRQ_FLAG_OFFSET             0x18

#define APB_RTC_STAT_REG_ADDR               (ARCHI_RTC_ADDR + APB_RTC_STAT_OFFSET)
#define APB_RTC_CTRL_REG_ADDR               (ARCHI_RTC_ADDR + APB_RTC_CTRL_OFFSET)
#define APB_RTC_DATA_REG_ADDR               (ARCHI_RTC_ADDR + APB_RTC_DATA_OFFSET)
#define APB_RTC_IRQ_CTRL_REG_ADDR           (ARCHI_RTC_ADDR + APB_RTC_IRQ_CTRL_OFFSET)
#define APB_RTC_IRQ_MASK_REG_ADDR           (ARCHI_RTC_ADDR + APB_RTC_IRQ_MASK_OFFSET)
#define APB_RTC_IRQ_FLAG_REG_ADDR           (ARCHI_RTC_ADDR + APB_RTC_IRQ_FLAG_OFFSET)

//  Bit field of APB registers
#define APB_RTC_NOIRQ_REQ                   0x00    //   APB_RTC_Status  [0:1]
#define APB_RTC_IRQ_REQ                     0x01    //   APB_RTC_Status  [0:1]
#define APB_RTC_READ                        0x00
#define APB_RTC_WRITE                       0x01
#define APB_IRQ_HIGH_LEVEL                  0x00    // The generated INT_APB is a high level
#define APB_IRQ_LOW_LEVEL                   0x01    // The generated INT_APB is a low level
#define APB_IRQ_HIGH_LEVEL_PULSE            0x02    // The generated INT_APB is a high level pulse with a duration of 1 CKIN cycle
#define APB_IRQ_LOW_LEVEL_PULSE             0x03    // The generated INT_APB is a low level pulse with a duration of 1 CKIN cycle
#define APB_WRITE_IRQ_MASKED                0x01    // 0x0 APB Write Operation Irq Enabled
#define APB_READ_IRQ_MASKED                 0x01    // 0x0 APB Read Operation Irq Enabled
#define APB_W_Flag                          0x02    // b'1 of APB_RTC_IRQ_FLAG_REG_ADDR
#define APB_R_Flag                          0x01    // b'0 of APB_RTC_IRQ_FLAG_REG_ADDR
#define APB_SR_MASK                         0x03

//*****************************************************
//  Address of Indirect Access Register
//*****************************************************
#define RTC_Status_Addr                     0x00
#define RTC_Ctrl_Addr                       0x01
#define RTC_Clk_Ctrl_Addr                   0x02
#define RTC_IRQ_Ctrl_Addr                   0x08
#define RTC_IRQ_Mask_Addr                   0x09
#define RTC_IRQ_Flag_Addr                   0x0A
#define RTC_Calendar_Ctrl_Addr              0x10
#define RTC_Calendar_TIME_Addr              0x12
#define RTC_Calendar_DATE_Addr              0x13
#define RTC_Alarm_Ctrl_Addr                 0x18
#define RTC_Alarm1_TIME_Addr                0x1A
#define RTC_Alarm1_DATE_Addr                0x1B
#define RTC_CntDown_Ctrl_Addr               0x20
#define RTC_CntDown1_Init_Addr              0x21
#define RTC_CntDown1_Timer_Addr             0x22
#define RTC_CKIN_DIV1_Addr                  0x28
#define RTC_Ref_Clk_Conf_Addr               0x2A
#define RTC_Test_Addr                       0x30

//  Bit field of RTC indirect Access Register
#define RTC_IRQ_REQ                         0x01    //   RTC_Status  [0:1]
#define RTC_Soft_Reset                      0x01    // 0x00: No soft reset
#define RTC_Calibration_Enable              0x01    // 0x00: Calibration disable
#define RTC_Active_Mode                     0x00
#define RTC_Standby_Mode                    0x01
#define RTC_CKOUT_Auto_Calibrated           0x01    // 0x00: CKOUT not auto calibrated
#define RTC_CKOUT_Not_Generated             0x01    // 0x00: CKOUT is generated
#define RTC_Irq_High_Level                  0x00    // The generated INT_RTC is a high level
#define RTC_Irq_Low_Level                   0x01    // The generated INT_RTC is a low level
#define RTC_Irq_High_Level_Pulse            0x02    // The generated INT_RTC is a high level pulse with a duration of 1 CKIN cycle
#define RTC_Irq_Low_Level_Pulse             0x03    // The generated INT_RTC is a low level pulse with a duration of 1 CKIN cycle
#define RTC_Calibration_Irq_Masked          0x01    // Calibration Irq Enabled
#define RTC_Timer1_Irq_Enable               0x00    // Countdown timer 1 irq enabled
#define RTC_Timer1_Irq_Masked               0x01    // Countdown timer 1 irq masked;
#define RTC_Alarm1_Irq_Enable               0x00    // Alarm 1 irq masked; When 0 Alarm 1 irq enabled
#define RTC_Alarm1_Irq_Masked               0x01    // Alarm 1 irq masked; When 0 Alarm 1 irq enabled
#define RTC_Calendar_Inactive               0x01    // 0x0: Calendar is active
#define RTC_Alarm_Rpt_Mode                  0x01    // 0x0: Single Mode
#define RTC_Alarm_ON                        0x00    // 0x0: Alarm1 is active
#define RTC_Alarm_OFF                       0x01    // 0x0: Alarm1 is active
#define RTC_CountDown1_Rpt_Mode             0x01    // 0x0: Single Mode
#define RTC_CountDown1_Active               0x00    // 0x0: Countdown 1 is active
#define RTC_CountDown1_Inactive             0x01

#define RTC_Irq_Alarm1_Flag                 0x01
#define RTC_Irq_Timer1_Flag                 0x10
#define RTC_Irq_Calibration_Flag            0x1000
#endif
