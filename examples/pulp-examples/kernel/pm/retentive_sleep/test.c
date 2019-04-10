/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "rt/rt_api.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#define RTC_TIME 2
#define GPIO_STATE 	17

#define DEBUG_PRINTF

extern rt_rtc_t dev_rtc;


unsigned int __rt_iodev=RT_IODEV_UART;
RT_L2_DATA rt_rtc_conf_t rtc_conf;
RT_L2_DATA rt_rtc_t *rtc;

void go_to_sleep(){

#ifdef DEBUG_PRINTF
	printf("Going to sleep!\n");
#endif
	rt_rtc_cntDwn_t value = { RTC_TIME, 0 };
	rt_rtc_control(rtc, RTC_CNTDOWN_SET, (void *)&value, NULL);
	rt_rtc_control(rtc, RTC_CNTDOWN_START, NULL, NULL);

	//set pin before going to sleep
  	rt_gpio_set_pin_value(0, GPIO_STATE, 0);

	if(rt_pm_state_switch(RT_PM_STATE_SLEEP, RT_PM_STATE_FAST)==-1)
		printf("Error....\n");

#ifdef DEBUG_PRINTF
	printf("Back from sleep!\n");
#endif
}

RT_L2_DATA int init = 0;

int main()
{
	//GPIO configutration
  	rt_gpio_init(0, GPIO_STATE);
  	rt_gpio_set_dir(0, 1<<GPIO_STATE, RT_GPIO_IS_OUT);
  	rt_gpio_set_pin_value(0, GPIO_STATE, 1);

	unsigned int * pads;
	unsigned int * pads_sleep;
	dev_rtc.open_count=0;

	//Enable isoltation mode
	//pads = (unsigned int *) 0x1A1041C0; //REG_GPIO_ISO
	//*pads = 0x01;
	//pads = (unsigned int *) 0x1A1041C4; //REG_CAM_ISO
	//*pads = 0x01;
	//pads = (unsigned int *) 0x1A1041C8; //REG_LVDS_ISO
	//*pads = 0x01;

	//PAD Config
/*	pads = (unsigned int *) 0x1A104150;
	*pads = 0x000000;
	pads = (unsigned int *) 0x1A104154;
	*pads = 0x000000;
	*/

	//B25 A22 high output
	//pads = (unsigned int *) 0x1A104158;
	//*pads = 0b1111 <<22;

	//Enable pad sleep mode
	//pads = (unsigned int *) 0x1A104160;
	//*pads = 0x1;



	//Pad sleep
  	/*pads = (unsigned int *) 0x1A104148;
  	*pads = 0x10000;*/
  	//hal_apb_soc_pad_set_function(40, 1);

	unsigned int instr, cycles;
    if (rt_event_alloc(NULL, 8)) return -1;

#ifdef DEBUG_PRINTF
    printf("\n\n*********************************\n");
    printf("         LCD - Retentive SLEEP Test\n");
    printf("*********************************\n\n");
#endif




	rtc_conf.clkDivider = 0x8000;


	rtc = rt_rtc_open(&rtc_conf, NULL);
	if (rtc == NULL) return -1;

	rt_rtc_cntDwn_t value = { RTC_TIME, 0 };
	rt_rtc_control(rtc, RTC_CNTDOWN_SET, (void *)&value, rt_event_get(NULL, go_to_sleep, rtc));
	rt_rtc_control(rtc, RTC_CNTDOWN_START, NULL, NULL);


    while(1){
#ifdef DEBUG_PRINTF
    	printf("halo\n");
#endif
	    rt_time_wait_us(1000000);
    }

   return 0;
}

