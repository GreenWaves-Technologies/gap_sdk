/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "lprnet.h"
#include "lprnetKernels.h"
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#ifdef __EMUL__
char *ImageName;
#endif

#define AT_INPUT_SIZE (AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR*AT_INPUT_COLORS_LPR)

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

static char *CHAR_DICT [71] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "<Anhui>", "<Beijing>", "<Chongqing>", "<Fujian>", "<Gansu>", "<Guangdong>", "<Guangxi>", "<Guizhou>", "<Hainan>", "<Hebei>", "<Heilongjiang>", "<Henan>", "<HongKong>", "<Hubei>", "<Hunan>", "<InnerMongolia>", "<Jiangsu>", "<Jiangxi>", "<Jilin>", "<Liaoning>", "<Macau>", "<Ningxia>", "<Qinghai>", "<Shaanxi>", "<Shandong>", "<Shanghai>", "<Shanxi>", "<Sichuan>", "<Tianjin>", "<Tibet>", "<Xinjiang>", "<Yunnan>", "<Zhejiang>", "<police>", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_"};

typedef signed char IMAGE_IN_T;
#ifdef __EMUL__
	unsigned char Input_1[AT_INPUT_SIZE];
#else
	unsigned char * Input_1;
	static struct pi_device dmacpy;
#endif
signed char * Output_1;

static void RunNetwork()
{
#ifdef PERF
	PRINTF("Start timer\n");
	gap_cl_starttimer();
	gap_cl_resethwtimer();
#endif
  __PREFIX(CNN)(Input_1, Output_1);

	PRINTF("Runner completed\n");
	int max_prob;
	int predicted_char = 70;
	PRINTF("OUTPUT: \n");
	for (int i=0; i<88; i++){
		max_prob = 0x80000000;
		for (int j=0; j<71; j++){
			if (Output_1[i+j*88]>max_prob){
				max_prob = Output_1[i+j*88];
				predicted_char = j;
			}
		}
		if (predicted_char==70) continue;
		PRINTF("%s, ", CHAR_DICT[predicted_char]);
	}
	PRINTF("\n");
}

int start()
{	
#ifndef __EMUL__
	#ifdef MEASUREMENTS
    pi_gpio_pin_configure(NULL, PI_GPIO_A0_PAD_8_A4, PI_GPIO_OUTPUT);
    pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 0);
    #endif

	/*-----------------------OPEN THE CLUSTER--------------------------*/
	struct pi_device cluster_dev;
	struct pi_cluster_conf conf;
	pi_cluster_conf_init(&conf);
	pi_open_from_conf(&cluster_dev, (void *)&conf);
	pi_cluster_open(&cluster_dev);

	pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
	pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);

	char *ImageName = __XSTR(AT_IMAGE);
	//Reading Image from Bridge
	uint8_t* Input_1 = (uint8_t*) pmsis_l2_malloc(AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR*3*sizeof(char));
	if(Input_1==NULL){
		PRINTF("Error allocating image buffer\n");
		pmsis_exit(-1);
	}
	/* -------------------- Read Image from bridge ---------------------*/
	PRINTF("Reading image\n");
	if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH_LPR, AT_INPUT_HEIGHT_LPR, 1, Input_1, AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
		printf("Failed to load image %s\n", ImageName);
		return 1;
	}
	for(int i=0; i<AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR; i++){
		int temp = Input_1[i] - 128;
		Input_1[i] = temp;
		Input_1[i+AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR] = temp;
		Input_1[i+2*AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR] = temp;
	}
	PRINTF("Finished reading image\n");
#else
	PRINTF("Reading image\n");
	if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH_LPR, AT_INPUT_HEIGHT_LPR, 1, Input_1, AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
		printf("Failed to load image %s\n", ImageName);
		return 1;
	}
	for (int i=0; i<AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR; i++){
		int temp = Input_1[i] - 128;
		Input_1[i]                                          = temp;
		Input_1[i+AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR]   = temp;
		Input_1[i+2*AT_INPUT_WIDTH_LPR*AT_INPUT_HEIGHT_LPR] = temp;
	}
#endif
	//Allocate output buffers:
	Output_1  = (char *)AT_L2_ALLOC(0, 71*88);
	if(Output_1==NULL){
		printf("Error Allocating CNN output buffers");
		return 1;
	}

	// IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
	if (__PREFIX(CNN_Construct)())
	{
		printf("Graph constructor exited with an error\n");
		return 1;
	}
	PRINTF("Graph constructor was OK\n");

#ifndef __EMUL__
	/*--------------------------TASK SETUP------------------------------*/
	struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
	if(task==NULL) {
		printf("pi_cluster_task alloc Error!\n");
		pmsis_exit(-1);
	}
	PRINTF("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
	memset(task, 0, sizeof(struct pi_cluster_task));
	task->entry = &RunNetwork;
	task->stack_size = STACK_SIZE;
	task->slave_stack_size = SLAVE_STACK_SIZE;
	task->arg = NULL;

	#ifdef MEASUREMENTS
	for (int i=0; i<1000; i++){
		pi_time_wait_us(50000);
	    pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 1);
		// Execute the function "RunNetwork" on the cluster.
		pi_cluster_send_task_to_cl(&cluster_dev, task);
	    pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 0);
    }
    #else
		// Execute the function "RunNetwork" on the cluster.
		pi_cluster_send_task_to_cl(&cluster_dev, task);
	#endif
#else
	RunNetwork();
#endif

#if 0
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
#endif

	__PREFIX(CNN_Destruct)();

#ifndef __EMUL__
	pmsis_exit(0);
#endif
	PRINTF("Ended\n");
	return 0;
}

#ifndef __EMUL__
int main(void)
{
    PRINTF("\n\n\t *** NNTOOL LPRNET ***\n\n");
  	return pmsis_kickoff((void *) start);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PRINTF("Usage: mnist [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    PRINTF("\n\n\t *** NNTOOL LPRNET emul ***\n\n");
    start();
    return 0;
}
#endif
