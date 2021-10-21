/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "ssdlite_ocr.h"
#include "ssdlite_ocrKernels.h"
#ifndef __EMUL__
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"
#endif
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#ifdef SILENT
  #define PRINTF(...) ((void) 0)
#else
  #define PRINTF printf
#endif

#define FIX2FP(Val, Precision)    ((float) (Val) / (float) (1<<(Precision)))

#define AT_INPUT_SIZE (AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD*AT_INPUT_COLORS_SSD)

#define MAX_BB          (300)
#define CAMERA_WIDTH    (324)
#define CAMERA_HEIGHT   (244)
#define CAMERA_COLORS   (1)
#define CAMERA_SIZE     (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_COLORS)

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

#ifdef __EMUL__
  char *ImageName;
  uint8_t Input_1[AT_INPUT_SIZE];
#else
  struct pi_device camera;
  struct pi_device HyperRam;
  struct pi_device ili;
  static pi_buffer_t buffer;
  static uint32_t l3_buff;
#endif

L2_MEM bbox_t *out_boxes;

#ifdef HAVE_LCD
static int open_display(struct pi_device *device)
{
  struct pi_ili9341_conf ili_conf;
  pi_ili9341_conf_init(&ili_conf);
  pi_open_from_conf(device, &ili_conf);
  if (pi_display_open(device))
    return -1;
  if (pi_display_ioctl(device, PI_ILI_IOCTL_ORIENTATION, (void *)PI_ILI_ORIENTATION_90))
    return -1;
  return 0;
}
#endif

#ifdef HAVE_HIMAX
static int open_camera_himax(struct pi_device *device)
{
  struct pi_himax_conf cam_conf;
  pi_himax_conf_init(&cam_conf);
  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;
  return 0;
}
#endif

static void RunNetwork()
{
  PRINTF("Running on cluster\n");
#ifdef PERF
  printf("Start timer\n");
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
#ifndef __EMUL__
  __PREFIX(CNN)(l3_buff, out_boxes);
#else
  __PREFIX(CNN)(Input_1, out_boxes);
#endif
}

int start()
{
#ifndef __EMUL__
  #ifdef MEASUREMENTS
  pi_gpio_pin_configure(NULL, PI_GPIO_A0_PAD_8_A4, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 0);
  #endif   
  #ifdef HAVE_HIMAX
    int err = open_camera_himax(&camera);
    if (err) {
      printf("Failed to open camera\n");
      pmsis_exit(-2);
    }
  #endif
  /* Init & open ram. */
  struct pi_hyperram_conf hyper_conf;
  pi_hyperram_conf_init(&hyper_conf);
  pi_open_from_conf(&HyperRam, &hyper_conf);
  if (pi_ram_open(&HyperRam))
  {
    printf("Error ram open !\n");
    pmsis_exit(-3);
  }

  if (pi_ram_alloc(&HyperRam, &l3_buff, (uint32_t) AT_INPUT_SIZE))
  {
    printf("Ram malloc failed !\n");
    pmsis_exit(-4);
  }

  pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
  pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);

/*-----------------------OPEN THE CLUSTER--------------------------*/
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  pi_cluster_conf_init(&conf);
  pi_open_from_conf(&cluster_dev, (void *)&conf);
  pi_cluster_open(&cluster_dev);

  #ifdef HAVE_LCD
    if (open_display(&ili)){
      printf("Failed to open display\n");
      pmsis_exit(-1);
    }
  #endif
  while(1){
    #ifdef HAVE_HIMAX
      uint8_t* Input_1 = (uint8_t*) pmsis_l2_malloc(CAMERA_SIZE*sizeof(char));
      //Reading Image from Bridge
      if(Input_1==NULL){
        printf("Error allocating image buffer\n");
        pmsis_exit(-1);
      }
      // Get an image 
      pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
      pi_camera_capture(&camera, Input_1, CAMERA_SIZE);
      pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
    #else
      uint8_t* Input_1 = (uint8_t*) pmsis_l2_malloc(AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD*sizeof(char));
      char *ImageName = __XSTR(AT_IMAGE);
      //Reading Image from Bridge
      if(Input_1==NULL){
        printf("Error allocating image buffer\n");
        pmsis_exit(-1);
      }
    /* -------------------- Read Image from bridge ---------------------*/
      PRINTF("Reading image\n");
      if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH_SSD, AT_INPUT_HEIGHT_SSD, 1, Input_1, AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
        printf("Failed to load image %s\n", ImageName);
        return 1;
      }
    #endif
    #ifdef HAVE_LCD
      #ifdef HAVE_HIMAX
        // Image Cropping to [ AT_INPUT_HEIGHT_SSD x AT_INPUT_WIDTH_SSD ]
        int idx=0;
        for(int i =0;i<CAMERA_HEIGHT;i++){
          for(int j=0;j<CAMERA_WIDTH;j++){
            if (i<AT_INPUT_HEIGHT_SSD && j<AT_INPUT_WIDTH_SSD){
              Input_1[idx] = Input_1[i*CAMERA_WIDTH+j];
            idx++;
            }
          }
        }
      #endif
      buffer.data = Input_1;
      buffer.stride = 0;
      // WIth Himax, propertly configure the buffer to skip boarder pixels
      pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, Input_1);
      pi_buffer_set_stride(&buffer, 0);
      pi_buffer_set_format(&buffer, AT_INPUT_WIDTH_SSD, AT_INPUT_HEIGHT_SSD, 1, PI_BUFFER_FORMAT_GRAY);
      pi_display_write(&ili, &buffer, 0, 0, AT_INPUT_WIDTH_SSD, AT_INPUT_HEIGHT_SSD);
    #endif
    for(int i=0; i<AT_INPUT_HEIGHT_SSD*AT_INPUT_WIDTH_SSD; i++){
      Input_1[i] -= 128;
    }
    pi_ram_write(&HyperRam, l3_buff                                         , Input_1, (uint32_t) AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD);
    pi_ram_write(&HyperRam, l3_buff+AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD  , Input_1, (uint32_t) AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD);
    pi_ram_write(&HyperRam, l3_buff+2*AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD, Input_1, (uint32_t) AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD);
    #ifdef HAVE_HIMAX
      pmsis_l2_malloc_free(Input_1, CAMERA_SIZE*sizeof(char));
    #else
      pmsis_l2_malloc_free(Input_1, AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD*sizeof(char));
    #endif
  PRINTF("Finished reading image\n");

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
#else
  PRINTF("Reading image\n");
  if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH_SSD, AT_INPUT_HEIGHT_SSD, 1, Input_1, AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
    printf("Failed to load image %s\n", ImageName);
    return 1;
  }
  for (int i=0; i<AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD; i++){
    int temp = Input_1[i] - 128;
    Input_1[i]                                          = temp;
    Input_1[i+AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD]   = temp;
    Input_1[i+2*AT_INPUT_WIDTH_SSD*AT_INPUT_HEIGHT_SSD] = temp;
  }
#endif

  //Allocate output buffers:
  out_boxes  = (short int *)AT_L2_ALLOC(0, MAX_BB*sizeof(bbox_t));
  if(out_boxes==NULL){
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

#ifndef __EMUL__
  #ifdef PERF
	{
		unsigned int TotalCycles = 0, TotalOper = 0;
		printf("\n");
		for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
			printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
			TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
		}
		printf("\n");
		printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
		printf("\n");
	}
  #endif
#endif
  __PREFIX(CNN_Destruct)();
  if(out_boxes[0].alive){
    #ifdef HAVE_LCD
      writeFillRect(&ili, (int)(FIX2FP(out_boxes[0].x,14)*320), (int)(FIX2FP(out_boxes[0].y,14)*240), 2, (int)(FIX2FP(out_boxes[0].h,14)*240), 0xFFFF);
      writeFillRect(&ili, (int)(FIX2FP(out_boxes[0].x,14)*320), (int)(FIX2FP(out_boxes[0].y,14)*240), (int)(FIX2FP(out_boxes[0].w,14)*320), 2, 0xFFFF);
      writeFillRect(&ili, (int)(FIX2FP(out_boxes[0].x,14)*320), (int)(FIX2FP(out_boxes[0].h+out_boxes[0].y,14)*240), (int)(FIX2FP(out_boxes[0].w,14)*320), 2, 0xFFFF);
      writeFillRect(&ili, (int)(FIX2FP(out_boxes[0].w+out_boxes[0].x,14)*320), (int)(FIX2FP(out_boxes[0].y,14)*240), 2, (int)(FIX2FP(out_boxes[0].h,14)*240), 0xFFFF);
    #endif
    PRINTF("draw.rectangle((%d,%d,%d,%d), outline=(255, 255, 0))\n",
        (int)(FIX2FP(out_boxes[0].x,14)*320),
        (int)(FIX2FP(out_boxes[0].y,14)*240),
        (int)(FIX2FP(out_boxes[0].w+out_boxes[0].x,14)*320),
        (int)(FIX2FP(out_boxes[0].h+out_boxes[0].y,14)*240)
        );
  }
  AT_L2_FREE(0, out_boxes, MAX_BB*sizeof(bbox_t));
  PRINTF("Ended\n");
#ifndef __EMUL__
  #ifdef PERF
    break;
  #endif
}
  pmsis_exit(0);
#endif
}

#ifndef __EMUL__
int main(void)
{
  PRINTF("\n\n\t *** OCR SSD ***\n\n");
  return pmsis_kickoff((void *) start);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PRINTF("Usage: ./exe [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    PRINTF("\n\n\t *** OCR SSD Emul ***\n\n");
    start();
    return 0;
}
#endif
