/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include <stdio.h>

#ifdef RGB
	#include "mobv2_035_rgbKernels.h"
#else
	#include "mobv2_1_bwKernels.h"
#endif

#include "gaplib/ImgIO.h"

#include "pmsis.h"
#include "bsp/ram/hyperram.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/bsp.h"
#include "bsp/buffer.h"
#include "bsp/camera.h"
#include "bsp/camera/himax.h"
#include "bsp/camera/gc0308.h"
#include "bsp/display/ili9341.h"

#define CAMERA_WIDTH    (324)
#define CAMERA_HEIGHT   (244)
#ifdef RGB
  #define CAMERA_COLORS (3)
  static pi_task_t task_gc_1;
  // Camera Buffer for async read from sensor
  L2_MEM uint8_t camera_buff[AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2];
#else
  #define CAMERA_COLORS (1)
  static pi_task_t task_himax;
  // Camera Buffer for async read from sensor
  L2_MEM uint8_t camera_buff[CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_COLORS];
#endif
#define IMAGE_SIZE 		(CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_COLORS)
#define AT_INPUT_SIZE 	(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

#ifndef HAVE_CAMERA
	#define __XSTR(__s) __STR(__s)
	#define __STR(__s) #__s 
#endif

static struct pi_device dmacpy;
static pi_buffer_t buffer;
struct pi_device camera;
struct pi_device ili;
#ifdef PERF
	L2_MEM rt_perf_t *cluster_perf;
#endif

// Softmax always outputs Q15 short int even from 8 bit input
L2_MEM short int *ResOut;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;
extern unsigned char * __restrict__ Input_1;


// camera and LCD utilities 
#ifdef HAVE_LCD
static int open_display(struct pi_device *device)
{
  struct pi_ili9341_conf ili_conf;
  pi_ili9341_conf_init(&ili_conf);
  pi_open_from_conf(device, &ili_conf);
  if (pi_display_open(device))
    return -1;
  if (pi_display_ioctl(device, PI_ILI_IOCTL_ORIENTATION, (void *)PI_ILI_ORIENTATION_180))
    return -1;
  return 0;
}

void draw_text(struct pi_device *display, const char *str, unsigned posX, unsigned posY, unsigned fontsize)
{
    writeFillRect(ili, 0, 340, posX, fontsize*8, 0xFFFF);
    setCursor(ili, posX, posY);
    writeText(ili, str, fontsize);
}
#endif

#ifdef HAVE_CAMERA

static int open_camera_himax(struct pi_device *device)
{
  struct pi_himax_conf cam_conf;
  pi_himax_conf_init(&cam_conf);
  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;
  return 0;
}

static int open_camera_rgb(struct pi_device *device)
{
    printf("Opening GC0308 camera\n");
    struct pi_gc0308_conf cam_conf;
    pi_gc0308_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
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
  __PREFIX(CNN)(ResOut);
}


int body(void)
{
  #ifdef MEASUREMENTS
		pi_gpio_pin_configure(NULL, PI_GPIO_A0_PAD_8_A4, PI_GPIO_OUTPUT);
		pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 0);
  #endif   
	char result_out[30];
	
	// Voltage-Frequency settings
	#if !FREQ_FC==50
    pi_freq_set(PI_FREQ_DOMAIN_FC,FREQ_FC*1000*1000);
    #endif
//	PMU_set_voltage(1200,0);

	// Open Camera & Display
#ifdef HAVE_LCD
	if (open_display(&ili)){
		printf("Failed to open display\n");
		pmsis_exit(-1);
	}
    writeFillRect(&ili, 0, 0, 240, 320, 0xFFFF);
    writeText(&ili, "GreenWaves Technologies", 2);
#endif

#ifdef HAVE_CAMERA
	#ifdef RGB
	  int err = open_camera_rgb(&camera);
	#else
	  int err = open_camera_himax(&camera);
	#endif
	if (err) {
		printf("Failed to open camera\n");
		pmsis_exit(-2);
	}
	#ifdef RGB
	    pi_camera_set_crop(&camera, (320-AT_INPUT_WIDTH)/2, (240-AT_INPUT_HEIGHT)/2, AT_INPUT_WIDTH, AT_INPUT_HEIGHT);
	#endif
#endif

	// Allocate Output Tensors
	ResOut = (short int *) AT_L2_ALLOC(0, 2*sizeof(short int));
	if (ResOut==0) {
		printf("Failed to allocate Memory for Result (%ld bytes)\n", 2*sizeof(short int));
		return 1;
	}

	// Open the Cluster
	struct pi_device cluster_dev;
	struct pi_cluster_conf conf;
	pi_cluster_conf_init(&conf);
	pi_open_from_conf(&cluster_dev, (void *)&conf);
	pi_cluster_open(&cluster_dev);
	pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
	// Tesk Setup
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

	// Construct the Graph
    PRINTF("Constructor\n");
	// IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
	int c_err;
	if (c_err=__PREFIX(CNN_Construct)())
	{
	  printf("Graph constructor exited with an error %d\n",c_err);
	  return 1;
	}
	PRINTF("Constructor was OK!\n");

	// Config Buffer for LCD Display 
	buffer.data = Input_1;
	buffer.stride = 0;

	// WIth Himax, propertly configure the buffer to skip boarder pixels
	pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, Input_1);//+AT_INPUT_WIDTH*2+2);
	pi_buffer_set_stride(&buffer, 0);
	#ifdef HAVE_CAMERA
	#ifdef RGB
		pi_buffer_set_format(&buffer, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, 2, PI_BUFFER_FORMAT_RGB565);
	    // start first aquisition -> async
	    //uDMA max transfer is 128KB but input is less (224*224*2[rgb565]) -> only one transfer
	    pi_camera_capture_async(&camera, camera_buff, AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2, pi_task_block(&task_gc_1));
	    pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
	#else
		pi_buffer_set_format(&buffer, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);
	    // start first aquisition -> async
	    pi_camera_capture_async(&camera, camera_buff, IMAGE_SIZE, pi_task_block(&task_himax));
	    pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
	#endif
    /* Init & open dmacpy. */
    struct pi_dmacpy_conf dmacpy_conf = {0};
    pi_dmacpy_conf_init(&dmacpy_conf);
    pi_open_from_conf(&dmacpy, &dmacpy_conf);
    int errors = pi_dmacpy_open(&dmacpy);
    if (errors)
    {
        printf("Error dmacpy open : %ld !\n", errors);
        pmsis_exit(-3);
    }
	#endif

/* ----------------------------------------------------------- MAIN LOOP ---------------------------------------------------------------- */
	int count = 0;
	int iterate = 1;
	while(iterate){
		/*------------------- reading input data -----------------------------*/
	    #ifdef HAVE_CAMERA
			#ifdef RGB
	            pi_task_wait_on(&task_gc_1);
	            /* Copy buffer from L2 to L2. */
			    errors = pi_dmacpy_copy(&dmacpy, (void *) camera_buff, (void *) Input_1, AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2, PI_DMACPY_L2_L2);
			    if(errors){
					printf("Copy from L2 to L2 failed : %ld\n", errors); pmsis_exit(-5);
			    }
	            pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
	            pi_camera_capture_async(&camera, camera_buff, AT_INPUT_WIDTH*AT_INPUT_HEIGHT*2, pi_task_block(&task_gc_1));
	            pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
		    #else
			    // wait previous async aquisition
			    pi_task_wait_on(&task_himax);
			    // Image Cropping to [AT_INPUT_HEIGHT x AT_INPUT_WIDTH]
			    int off_src = 0, off_dst = 0;
			    for (int i=0; i<AT_INPUT_HEIGHT; i++){
			    	/* Copy buffer from L2 to L2. */
				    errors = pi_dmacpy_copy(&dmacpy, (void *) camera_buff+off_src, (void *) Input_1+off_dst, AT_INPUT_WIDTH, PI_DMACPY_L2_L2);
				    if(errors){
						printf("Copy from L2 to L2 failed : %ld\n", errors); pmsis_exit(-5);
				    }
				    off_src += CAMERA_WIDTH; off_dst += AT_INPUT_WIDTH;
			    }
			    // start next aquisition
			    pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
			    pi_camera_capture_async(&camera, camera_buff, IMAGE_SIZE, pi_task_block(&task_himax));
			    pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
		  	#endif //CAMERA_TYPE
		#else
			iterate=0; //do not iterate if there is no camera
			char *ImageName = __XSTR(AT_IMAGE);
			PRINTF("Reading image from %s\n",ImageName);
			//Reading Image from Bridge
			#ifdef RGB
		  		img_io_out_t type = IMGIO_OUTPUT_RGB565;
			#else
		  		img_io_out_t type = IMGIO_OUTPUT_CHAR;  		
			#endif
			if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Input_1, AT_INPUT_SIZE*sizeof(unsigned char), type, 0)) {
				printf("Failed to load image %s\n", ImageName);
				return 1;
			}
		#endif //HAVE_CAMERA

		#ifdef HAVE_LCD
			pi_display_write(&ili, &buffer, 8, 20, AT_INPUT_WIDTH, AT_INPUT_HEIGHT);
		#endif

  		// send task to cluster
		#ifdef MEASUREMENTS
			pi_time_wait_us(1000);
			pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 1);
		#endif
        pi_task_t task_cl;
		pi_cluster_send_task_to_cl_async(&cluster_dev, task, pi_task_block(&task_cl));
	    pi_task_wait_on(&task_cl);
	    #ifdef MEASUREMENTS
			pi_gpio_pin_write(NULL, PI_GPIO_A0_PAD_8_A4, 0);
		#endif

		// check results
		float vehicle_not_seen = FIX2FP(ResOut[0], 15);    
        float vehicle_seen = FIX2FP(ResOut[1], 15);
	  	#ifdef HAVE_LCD
	        if (vehicle_seen > vehicle_not_seen) {
	            sprintf(result_out,"YES (%f)",vehicle_seen);
				writeFillRect(&ili, 0, 270, 244, 50, 0x07E0);
	            draw_text(&ili, result_out, 30, 250, 2);
	            }
	        else{
	            sprintf(result_out,"NO  (%f)",vehicle_not_seen);
				writeFillRect(&ili, 0, 270, 244, 50, 0xF800);
	            draw_text(&ili, result_out, 30, 250, 2);
	        }
	    #else
	        if (vehicle_seen > vehicle_not_seen) {
	            PRINTF("vehicle seen! confidence %f\n", vehicle_seen);
    			// pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
	        } else {
	            PRINTF("no vehicle seen %f\n", vehicle_not_seen);
    			// pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
	        }
	  	#endif

		#ifdef PERF
			// Performance Counters
			{
				unsigned int TotalCycles = 0, TotalOper = 0;
				printf("\n");
				for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
					printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
					TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
				}
				printf("\n");
				printf("\t\t\t %s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
				printf("\n");
			}
		#endif
	}

		// Desctruct the AT model
		__PREFIX(CNN_Destruct)();

	PRINTF("Ended\n");
	pmsis_exit(0);	
	return 0;
}

int main(void)
{
    PRINTF("\n\n\t *** Visualwakewords for vehicle ***\n\n");
    return pmsis_kickoff((void *) body);
}
