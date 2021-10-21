/*
 * Copyright (c) 2019-2020 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Gap.h"

#include "pmsis.h"
#include "bsp/buffer.h"
#include "bsp/display/ili9341.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/gc0308.h"

#include "gaplib/ImgIO.h"
#include "ResizeBasicKernels.h"
#include "vww17Kernels.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#define INPUT_SIZE    (INPUT_WIDTH*INPUT_HEIGHT*1)
#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*1)
#define HEAD_DETECTOR_THRESHOLD -44 //(0.1)
#define LOOK_THRESHOLD  13 //(0.68)
#define NEW_WIDTH                220
#define NEW_HEIGHT               165
#define ENSAMBLE_ITERATIONS      2

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;
signed char Out_Head[1];
signed char Out_Look[1];
signed char * Input_Image;
struct pi_device HyperRam;
static uint32_t l3_buff;
struct pi_device ili;
struct pi_device camera;
static pi_buffer_t buffer;

struct pi_cluster_task task_network;
struct pi_cluster_task task_resize;

#ifdef HAVE_LCD
static int open_display(struct pi_device *device)
{
  struct pi_ili9341_conf ili_conf;
  pi_ili9341_conf_init(&ili_conf);
  pi_open_from_conf(device, &ili_conf);
  if (pi_display_open(device))
    return -1;
  if (pi_display_ioctl(device, PI_ILI_IOCTL_ORIENTATION, (void *)PI_ILI_ORIENTATION_0))
    return -1;
  return 0;
}
void draw_text(struct pi_device *display, const char *str, unsigned posX, unsigned posY, unsigned fontsize)
{
    writeFillRect(display, posX, posY, 240, fontsize*10, 0xFFFF);
    setCursor(display, posX, posY);
    setTextColor(display, 0x03E0);
    writeText(display, str, fontsize);
}      
static void add_gwt_logo(struct pi_device* display)
{
    setCursor(display, 30, 0);
    writeText(display, "GreenWaves\0", 3);
    setCursor(display, 10, 3*8);
    writeText(display, "Technologies\0", 3);
}
#endif

#ifdef HAVE_HIMAX
static int open_camera_himax(struct pi_device *device)
{
    PRINTF("Opening Himax camera\n");
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
       return -1;
    return 0;
}
#endif
#ifdef HAVE_GC
static int open_camera_gc(struct pi_device *device)
{
    PRINTF("Opening GC0308 camera\n");
    struct pi_gc0308_conf cam_conf;
    pi_gc0308_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
    cam_conf.color_mode = PI_CAMERA_GRAY8;
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
        return -1;
    return 0;
}
#endif

static void Resize(KerResizeBilinear_ArgT *KerArg)
{
    PRINTF("Resizing...\n");
    AT_FORK(gap_ncore(), (void *) KerResizeBilinear, (void *) KerArg);
}

static void RunNetwork()
{
  PRINTF("Running on cluster\n");
#ifdef PERF
  printf("Start timer\n");
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  vww17CNN(l3_buff, Out_Head, Out_Look);
}

void body()
{
    pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
    char string_buffer[64];

    #if defined(HAVE_HIMAX) || defined(HAVE_GC)
        #ifdef HAVE_HIMAX
        int err = open_camera_himax(&camera);
        #endif
        #ifdef HAVE_GC
        int err = open_camera_gc(&camera);
        #endif
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
      writeFillRect(&ili, 0, 0, 240, 320, 0xFFFF);
      add_gwt_logo(&ili);
    #endif

    int iter, Mean_Out_Head, Mean_Out_Look = 0;
    while(1){
        uint8_t* Input_Image = (uint8_t*) pmsis_l2_malloc(INPUT_SIZE*sizeof(char));
        if(Input_Image==0) {
            printf("Input_Image alloc Error!\n");
            pmsis_exit(-1);
        }
        #if defined(HAVE_HIMAX) || defined(HAVE_GC)
            // Get an image 
            pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
            pi_camera_capture(&camera, Input_Image, INPUT_SIZE);
            pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
        #else
            char *ImageName = __XSTR(AT_IMAGE);
            PRINTF("Reading image\n");
            if (ReadImageFromFile(ImageName, INPUT_WIDTH, INPUT_HEIGHT, 1, Input_Image, INPUT_SIZE*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
                printf("Failed to load image %s\n", ImageName);
                pmsis_exit(-1);
            }
        #endif
        ////////////////// RESIZER BILINEAR !!!!!!!!!!!!!!!!!!!!!!!!!!!
        // /*--------------------------TASK SETUP------------------------------*/
        task_resize.entry = &Resize;
        task_resize.stack_size = 1024;
        task_resize.slave_stack_size = 512;

        uint8_t* Image_Resized = (uint8_t*) pmsis_l2_malloc(NEW_WIDTH*NEW_HEIGHT*sizeof(char));
        if(Image_Resized==0) {
            printf("Image_Resized alloc Error!\n");
            pmsis_exit(-1);
        }
        KerResizeBilinear_ArgT ResizeArg;
            ResizeArg.In             = Input_Image;
            ResizeArg.Win            = INPUT_WIDTH;
            ResizeArg.Hin            = INPUT_HEIGHT;
            ResizeArg.Out            = Image_Resized;
            ResizeArg.Wout           = NEW_WIDTH;
            ResizeArg.Hout           = NEW_HEIGHT;
            ResizeArg.HTileOut       = NEW_HEIGHT;
            ResizeArg.FirstLineIndex = 0;
        task_resize.arg = &ResizeArg;
        pi_cluster_send_task_to_cl(&cluster_dev, &task_resize);
        PRINTF("Resized\n");

        int offset_w = (NEW_WIDTH - AT_INPUT_WIDTH) >> 1;
        int offset_h = (NEW_HEIGHT - AT_INPUT_HEIGHT) >> 1;
        // MANUAL 2D Copy
        for(int i=0; i<AT_INPUT_HEIGHT; i++){
            pi_ram_write(&HyperRam, l3_buff+i*AT_INPUT_WIDTH, Image_Resized+offset_w+(i+offset_h)*NEW_WIDTH, AT_INPUT_WIDTH);
        }
        PRINTF("Finished copy to ram\n");
        //pi_ram_copy_2d(&HyperRam, l3_buff, Image_Resized+offset, (uint32_t) AT_INPUT_SIZE, NEW_WIDTH, AT_INPUT_WIDTH, 0);
        //pi_ram_write(&HyperRam, l3_buff, Input_Image, (uint32_t) AT_INPUT_SIZE);
        pmsis_l2_malloc_free(Image_Resized, NEW_WIDTH*NEW_HEIGHT*sizeof(char));
        pmsis_l2_malloc_free(Input_Image, INPUT_SIZE*sizeof(char));
        PRINTF("Finished malloc free\n");

        #ifdef HAVE_LCD
            uint8_t* lcd_buffer = (uint8_t*) pmsis_l2_malloc(AT_INPUT_SIZE*sizeof(char));
            pi_ram_read(&HyperRam, l3_buff, lcd_buffer, AT_INPUT_SIZE);
            buffer.data = lcd_buffer;
            buffer.stride = 0;
            // WIth Himax, propertly configure the buffer to skip boarder pixels
            pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, lcd_buffer);
            pi_buffer_set_stride(&buffer, 0);
            pi_buffer_set_format(&buffer, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);
            pi_display_write(&ili, &buffer, (INPUT_HEIGHT-AT_INPUT_WIDTH)>>1, 106, AT_INPUT_WIDTH, AT_INPUT_HEIGHT);        
            pmsis_l2_malloc_free(lcd_buffer, AT_INPUT_SIZE*sizeof(char));
        #endif
        PRINTF("Finished lcd display");

        /*--------------------------TASK SETUP------------------------------*/
        task_network.entry = &RunNetwork;
        task_network.stack_size = STACK_SIZE;
        task_network.slave_stack_size = SLAVE_STACK_SIZE;
        task_network.arg = NULL;

        if (__PREFIX(CNN_Construct)())
        {
            printf("Graph constructor exited with an error\n");
            pmsis_exit(-1);
        }
        PRINTF("vww17CNN_Construct..done\n");

        pi_cluster_send_task_to_cl(&cluster_dev, &task_network);
        PRINTF("HEAD: %d LOOK: %d\n", Out_Head[0], Out_Look[0]);
        Mean_Out_Head += Out_Head[0];
        Mean_Out_Look += Out_Look[0];
        iter++;

        if (!(iter % ENSAMBLE_ITERATIONS)){
            Mean_Out_Head /= ENSAMBLE_ITERATIONS;
            Mean_Out_Look /= ENSAMBLE_ITERATIONS;
            if(Mean_Out_Head > HEAD_DETECTOR_THRESHOLD)
            {
                sprintf(string_buffer, "HEAD (%d)", Mean_Out_Head);
                #ifdef HAVE_LCD
                    writeFillRect(&ili, 28, 240, 240, 45, 0xFFFF);
                    draw_text(&ili, string_buffer, 50, 240, 2);
                #endif
                if(Out_Look[0] > LOOK_THRESHOLD)
                {
                    sprintf(string_buffer, "LOOKING (%d)", Mean_Out_Look);
                }
                else
                {
                    sprintf(string_buffer, "NOT LOOKING (%d)", Mean_Out_Look);
                }
                #ifdef HAVE_LCD
                    draw_text(&ili, string_buffer, 28, 255, 2);
                #endif
                PRINTF(string_buffer);
            }
            else
            {
                sprintf(string_buffer, "NO HEAD (%d)\n", Mean_Out_Head);
                #ifdef HAVE_LCD
                    writeFillRect(&ili, 28, 240, 240, 45, 0xFFFF);
                    draw_text(&ili, string_buffer, 30, 240, 2);
                #endif
                PRINTF(string_buffer);
            }
            Mean_Out_Head = 0;
            Mean_Out_Look = 0;
            iter = 0;
        }

        #ifdef PERF
        /*------------------------Performance Counter------------------------*/
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
        vww17CNN_Destruct();
    }
    pmsis_exit(0);
}


int main(int argc, char *argv[])
{
  PRINTF("\n\n\t *** OCR SSD ***\n\n");
  return pmsis_kickoff((void *) body);
}
