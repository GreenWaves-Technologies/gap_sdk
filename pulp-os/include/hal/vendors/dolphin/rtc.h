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

#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__

#include <hal/pulp.h>
#include "archi/vendors/dolphin/rtc.h"

/*  Structure for APB RTC registers  */
typedef struct {
    unsigned int apb_addr:6;           // Indirect access address, address range [0:3F]
    unsigned int pad:10;
    unsigned int apb_load:1;           /* Indirect access load bit:
                                            0: APB read operation
                                            1: APB write operation
                                       */
    unsigned int pad1:15;
}Apb_rtc_ctrlT;

typedef struct {
    unsigned int irq_form:2;
    unsigned int pad:30;
}Apb_irq_ctrlT;

typedef struct {
    unsigned int readMask:1;
    unsigned int writeMask:1;
    unsigned int pad:30;
}Apb_irq_maskT;

typedef union{
    Apb_rtc_ctrlT       rtcCtrl;
    Apb_irq_ctrlT       irqCtrl;
    Apb_irq_maskT       irqMask;
    unsigned int        Raw;
}Apb_rtcT;

//=============================================================
/*  Structure for indirect access registers  */

typedef struct{
    unsigned int rtc_sb:1;
    unsigned int pad:3;
    unsigned int cal_En:1;
    unsigned int pad1:3;
    unsigned int soft_rst:1;
    unsigned int pad2:23;
}Rtc_ctrlT;

typedef struct{
    unsigned int clkOut_sb:1;
    unsigned int pad:11;
    unsigned int div1_autoCal:1;
    unsigned int pad1:3;
    unsigned int div1_comp:5;
    unsigned int pad2:11;
}Rtc_clk_ctrlT;

typedef struct{
    unsigned int irq_form:2;
    unsigned int pad:30;
}Rtc_irq_ctrlT;

typedef struct{
    unsigned int alarm1_masked:1;
    unsigned int pad:3;
    unsigned int timer1_masked:1;
    unsigned int pad1:7;
    unsigned int cal_masked:1;
    unsigned int pad2:19;
}Rtc_irq_maskT;

typedef struct{
    unsigned int calend_En:1;
    unsigned int pad:31;
}Rtc_calend_ctrlT;

typedef struct{
    unsigned int alarm1_En:1;
    unsigned int pad:3;
    unsigned int alarm1_mode:1;
    unsigned int pad1:11;
    unsigned int alarm1_conf:4;
    unsigned int pad2:12;
}Rtc_alarm_ctrlT;

typedef struct{
    unsigned int cntDwn1_En:1;
    unsigned int pad:3;
    unsigned int cntDwn1_mode:1;
    unsigned int pad1:27;
}Rtc_cntDwn_ctrlT;

typedef struct {
    unsigned int divVal:16;
    unsigned int pad:16;
}Rtc_CKIN_divT;

typedef struct{
    unsigned int refClkVal:22;
    unsigned int pad:10;
}Rtc_ref_clkT;

typedef union{
//    unsigned int        status;
    Rtc_ctrlT           conf;
    Rtc_clk_ctrlT       clkConf;
    Rtc_irq_ctrlT       irqConf;
    Rtc_irq_maskT       irqMask;
//    unsigned int        irqFlag;
    Rtc_calend_ctrlT    calendConf;
//    unsigned int        calendTime;
//    unsigned int        calendDate;
    Rtc_alarm_ctrlT     alarmCtrl;
//    unsigned int        alarmTime;
//    unsigned int        alarmDate;
    Rtc_cntDwn_ctrlT    cntDwnCtrl;
//    unsigned int        cntDwn1InitVal;
//    unsigned int        cntDwn1Val;
    Rtc_CKIN_divT       ckinDiv;
    Rtc_ref_clkT        refClk;
    unsigned int        Raw;
}RtcT;

//=============================================================
/*
 * For Configuring the RTC, we use the APB interface:
        * Write a Register:
            1)   Write the data (the value of this indirect Access Register) to APB_RTC_DATA_REG_ADDR
            2)   Write the register address in APB_RTC_CTRL_REG_ADDR[0:5]
            3)   Set the write bit APB_RTC_CTRL_REG_ADDR[16] to 1

        * Read a Register:
            1)   Write the register address in APB_RTC_CTRL_REG_ADDR[0:5]
            2)   Set the write bit APB_RTC_CTRL_REG_ADDR[16] to 0
            3)   Read the value of the register from APB_RTC_DATA_REG_ADDR
 */

static inline unsigned int rt_apb_rtc_reg_read (unsigned int addr){
    return pulp_read32(addr);
}

static inline void rt_apb_rtc_reg_write(unsigned int addr, unsigned int conf){
    pulp_write32(addr, conf);
}

#endif
