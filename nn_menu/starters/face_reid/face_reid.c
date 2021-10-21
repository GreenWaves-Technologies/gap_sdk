/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/gc0308.h"
#include "bsp/display/ili9341.h"
#include "face_reid.h"
#include "face_reidInfo.h"
#include "face_reidKernels.h"
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)
#define PIXEL_SIZE 1

typedef struct{
    unsigned short* in;
    unsigned short* out;
} cluster_arg_t;

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

static uint32_t l3_buff;
struct pi_device gpio_a1;

struct pi_device ili;
struct pi_device device;
static pi_buffer_t buffer;

static struct pi_device camera;

#if SILENT
#define PRINTF(...) ((void) 0)
#else
#define PRINTF printf
#endif
// Softmax always outputs Q15 short int even from 8 bit input
PI_L2 short int *ResOut;
uint8_t *Input_1;


static void RunNetwork(cluster_arg_t*arg)
{
  //PRINTF("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  __PREFIX(CNN)(arg->in,arg->out);
  //PRINTF("Runner completed\n");
}

int start()
{
    char *ImageName = __XSTR(AT_IMAGE);
    struct pi_device cluster_dev;
    struct pi_cluster_task *task;
    struct pi_cluster_conf conf;
    char result_out[30];
    unsigned int W = 128, H = 128;
    unsigned int Wcam=238, Hcam=208;
    pi_task_t task_1;
    pi_task_t task_2;
    cluster_arg_t arg;
    pi_task_t wait_task;
    
    //Input image size
    PRINTF("Entering main controller\n");
    
    PMU_set_voltage(1000,0);
    //#if !FREQ_FC==50
    pi_freq_set(PI_FREQ_DOMAIN_FC,50*1000*1000);
    //#endif

    //Allocating output
    ResOut = (short int *) pmsis_l2_malloc( 512*sizeof(short int));
    if (ResOut==0) {
        printf("Failed to allocate Memory for Result (%ld bytes)\n", 2*sizeof(short int));
        pmsis_exit(-1);
    }

#ifndef FROM_CAMERA
    //allocating input
    Input_1 = (uint8_t*)pmsis_l2_malloc(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*PIXEL_SIZE);
    if (Input_1==0) {
        printf("Failed to allocate Memory for input (%ld bytes)\n", AT_INPUT_WIDTH*AT_INPUT_HEIGHT*PIXEL_SIZE);
        pmsis_exit(-1);
    }

    PRINTF("Reading image\n");
    //Reading Image from Bridge
    if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Input_1, AT_INPUT_SIZE*PIXEL_SIZE, IMGIO_OUTPUT_CHAR, 0)) {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-1);
    }
    PRINTF("Finished reading image\n");
#else
    //Allocate double the buffer for double buffering
    Input_1 = (uint8_t*)pmsis_l2_malloc(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*PIXEL_SIZE *2);
    if (Input_1==0) {
        printf("Failed to allocate Memory for input (%ld bytes)\n", AT_INPUT_WIDTH*AT_INPUT_HEIGHT*PIXEL_SIZE*2);
        pmsis_exit(-1);
    }

    if (open_display(&ili))
    {
        printf("Failed to open display\n");
        pmsis_exit(-1);
    }

    writeFillRect(&ili, 0, 0, 320, 240, 0xFFFF);
    writeText(&ili, "  GreenWaves Technologies", 2);

    buffer.data = Input_1;
    buffer.stride = 0;

    // WIth Himax, propertly configure the buffer to skip boarder pixels
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, Input_1);
    pi_buffer_set_stride(&buffer, 0);
    pi_buffer_set_format(&buffer, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, 2, PI_BUFFER_FORMAT_RGB565);

    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        pmsis_exit(-1);
    }
    pi_camera_set_crop(&camera,(320-AT_INPUT_WIDTH)/2,(240-AT_INPUT_HEIGHT)/2,AT_INPUT_WIDTH,AT_INPUT_HEIGHT);

    
#endif

    pi_cluster_conf_init(&conf);
    pi_open_from_conf(&cluster_dev, (void *)&conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-7);
    }
    
    task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = &RunNetwork;
    task->stack_size = CLUSTER_STACK_SIZE;
    task->slave_stack_size = CLUSTER_SLAVE_STACK_SIZE;
    task->arg = &arg;
    
    PRINTF("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (__PREFIX(CNN_Construct)())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-1);
    }

    pi_freq_set(PI_FREQ_DOMAIN_CL,50*1000*1000);

    PRINTF("Application main cycle\n");
    #ifdef FROM_CAMERA
    pi_camera_capture_async(&camera, Input_1, AT_INPUT_WIDTH*AT_INPUT_HEIGHT,pi_task_block(&task_1));
    pi_camera_capture_async(&camera, Input_1+AT_INPUT_WIDTH*AT_INPUT_HEIGHT, AT_INPUT_WIDTH*AT_INPUT_HEIGHT,pi_task_block(&task_2));
    pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
    #endif

    #ifdef GPIO 
    //GPIO A1 (A0 on board)
    pi_pad_set_function(PI_PAD_12_A3_RF_PACTRL0, PI_PAD_12_A3_GPIO_A0_FUNC1);
    pi_gpio_e gpio_out_a1 = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_flags_e cfg_flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_a1, gpio_out_a1, cfg_flags);
    #endif

    int iter=1;
    do{

        #ifndef FROM_CAMERA
            iter=0;
            arg.in=Input_1;
            arg.out=ResOut;
        #else
            pi_task_wait_on(&task_1);
            pi_task_wait_on(&task_2);
            pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
            //We need to calls since uDMA max transfer is 128KB
            pi_camera_capture_async(&camera, Input_1 + (iter%2?AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2:0), AT_INPUT_WIDTH*AT_INPUT_HEIGHT,pi_task_block(&task_1));
            pi_camera_capture_async(&camera, Input_1+(iter%2?AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2:0)+AT_INPUT_WIDTH*AT_INPUT_HEIGHT, AT_INPUT_WIDTH*AT_INPUT_HEIGHT,pi_task_block(&task_2));
            pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
            arg.in=Input_1+(iter%2?0:AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2);
            arg.out=ResOut;
        #endif
    
        #ifdef GPIO 
        pi_gpio_pin_write(&gpio_a1, gpio_out_a1, 1);
        #endif
        // Execute the function "RunNetwork" on the cluster.
        pi_task_block(&wait_task);
        pi_cluster_send_task_to_cl_async(&cluster_dev, task,&wait_task);

        
        #ifndef FROM_CAMERA
        pi_task_wait_on(&wait_task);
        #ifdef GPIO 
        pi_gpio_pin_write(&gpio_a1, gpio_out_a1, 0);
        #endif
        /*float person_not_seen = FIX2FP(ResOut[0] * S68_Op_output_1_OUT_QSCALE, S68_Op_output_1_OUT_QNORM);
        float person_seen = FIX2FP(ResOut[1] * S68_Op_output_1_OUT_QSCALE, S68_Op_output_1_OUT_QNORM);

        if (person_seen > person_not_seen) {
            PRINTF("person seen! confidence %f\n", person_seen);
        } else {
            PRINTF("no person seen %f\n", person_not_seen);
        }
        */#else
        buffer.data = arg.in;
        //Write to image to LCD while processing NN on cluster
        pi_display_write(&ili, &buffer, 41,16, AT_INPUT_WIDTH, AT_INPUT_HEIGHT);
        //Wait Cluster to finish befor writing results to LCD
        pi_task_wait_on(&wait_task);
        float person_not_seen = FIX2FP(ResOut[0] * S68_Op_output_1_OUT_QSCALE, S68_Op_output_1_OUT_QNORM);
        float person_seen = FIX2FP(ResOut[1] * S68_Op_output_1_OUT_QSCALE, S68_Op_output_1_OUT_QNORM);
        if (person_seen > person_not_seen) 
        {
            sprintf(result_out,"Person seen (%f)",person_seen);
            draw_text(&ili, result_out, 40, 225, 2);
        }
        else
        {
            sprintf(result_out,"No person seen (%f)",person_not_seen);
            draw_text(&ili, result_out, 40, 225, 2);
        }
        #endif
    }while(iter++);

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
    //Checks for jenkins:
    if(ResOut[0] == 4982 && ResOut[1] ==  27785) { printf("Correct Results!\n");pmsis_exit(0);}
    else { printf("Wrong Results!\n");pmsis_exit(-1);}
        
    pmsis_l2_malloc_free(ResOut, 2*sizeof(short int));
    pmsis_l2_malloc_free(Input_1,AT_INPUT_WIDTH*AT_INPUT_HEIGHT*PIXEL_SIZE);
    PRINTF("Ended\n");
    pmsis_exit(0);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void *) start);
}
