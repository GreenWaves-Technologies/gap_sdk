/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "vww.h"
#include "vwwKernels.h"
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#ifdef __EMUL__
#ifdef PERF
#undef PERF
#endif
#endif

#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

#ifndef STACK_SIZE
#define STACK_SIZE     2048 
#endif

#define LED 19

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

// Softmax always outputs Q15 short int even from 8 bit input
PI_L2 signed short int Output_1[2];
typedef signed char IMAGE_IN_T;
PI_L2 unsigned char Input_1[AT_INPUT_SIZE];

static void RunNetwork()
{
#ifdef PERF
	gap_cl_starttimer();
	gap_cl_resethwtimer();
#endif
	__PREFIX(CNN)(Input_1, Output_1);
}

void start()
{
	char *ImageName = __XSTR(AT_IMAGE);
	struct pi_device cluster_dev;
	struct pi_cluster_task *task;
	struct pi_cluster_conf conf;
	//Input image size

	printf("Entering main controller\n");

	printf("Reading image\n");
	//Reading Image from Bridge
	if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS,
			      		  Input_1, AT_INPUT_SIZE*sizeof(IMAGE_IN_T), IMGIO_OUTPUT_CHAR, 0)) {
		printf("Failed to load image %s\n", ImageName);
		pmsis_exit(-2);
	}
	printf("Finished reading image\n");


	pi_cluster_conf_init(&conf);
	pi_open_from_conf(&cluster_dev, (void *)&conf);
	pi_cluster_open(&cluster_dev);
	pi_freq_set(PI_FREQ_DOMAIN_CL,175000000);
	pi_freq_set(PI_FREQ_DOMAIN_FC,250000000);
	//PMU_set_voltage(1150,0);
	//PMU_set_voltage(1200,0);
	task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
	if (!task) {
		printf("failed to allocate memory for task\n");
	}
	pi_cluster_task(task, &RunNetwork, NULL);
	task->stack_size = STACK_SIZE;
	task->slave_stack_size = SLAVE_STACK_SIZE;

	printf("Constructor\n");

	// IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
	if (__PREFIX(CNN_Construct)())
	{
		printf("Graph constructor exited with an error\n");
		pmsis_exit(-1);
	}

	printf("Call cluster\n");
	
	// Execute the function "RunNetwork" on the cluster.
#ifdef __GAP8__
	pi_pad_set_function(PI_PAD_33_B12_TIMER0_CH2, PI_PAD_33_B12_GPIO_A19_FUNC1);
	pi_gpio_pin_configure(NULL, LED, PI_GPIO_OUTPUT);
	pi_gpio_pin_write(NULL, LED, 1);
#endif
	pi_cluster_send_task(&cluster_dev, task);
#ifdef __GAP8__
	pi_gpio_pin_write(NULL, LED, 0);
#endif

	//Check Results
	
	if (Output_1[1] > Output_1[0]) {
		printf("person seen (%d, %d)\n", Output_1[0], Output_1[1]);
	} else {
		printf("no person seen (%d, %d)\n", Output_1[0], Output_1[1]);
	}
	printf("\n");

	__PREFIX(CNN_Destruct)();

#ifdef PERF
	{
		unsigned int TotalCycles = 0, TotalOper = 0;
		printf("\n");
		for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
			printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i],
			       AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
			TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
		}
		printf("\n");
		printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
		printf("\n");
	}
#endif

	printf("Ended\n");
	pmsis_exit(0);
}

int main(void)
{
	return pmsis_kickoff((void *) start);
}
