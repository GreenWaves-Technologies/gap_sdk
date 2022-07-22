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

/*
 * This example shows how to capture images from the camera and display them on the LCD on the Gapuino board.
 */

#include "pmsis.h"
#include "bsp/display/ili9341.h"
#include "bsp/camera/himax.h"
#include "bsp/camera/mt9v034.h"
#include "stdio.h"

// Himax camera is having 324x244 images
#define CAM_WIDTH    324
#define CAM_HEIGHT   244

#define LCD_WIDTH    320
#define LCD_HEIGHT   240

static pi_task_t task;
static unsigned char *imgBuff0;
static struct pi_device ili;
static pi_buffer_t buffer;
static struct pi_device device;


static void lcd_handler(void *arg);
static void cam_handler(void *arg);


static int nb_frame = NB_FRAME;

// This handler is called as soon as one image is captured from the camera
static void cam_handler(void *arg)
{
  // Stop the camera stream as we just need one image
  pi_camera_control(&device, PI_CAMERA_CMD_STOP, 0);

  // And display the captured frame on the display
  pi_display_write_async(&ili, &buffer, 0, 0, LCD_WIDTH, LCD_HEIGHT, pi_task_callback(&task, lcd_handler, NULL));
}



// This handler is called as soon as the LCD driver has finished displayed the frame
static void lcd_handler(void *arg)
{
  // Check if we should continue
  if (nb_frame > 0)
  {
    nb_frame--;
    if (nb_frame == 0)
      return;
  }


  // Start again the camera image and capture one image
  pi_camera_control(&device, PI_CAMERA_CMD_START, 0);
  pi_camera_capture_async(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT, pi_task_callback(&task, cam_handler, NULL));
}



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


static int open_camera_himax(struct pi_device *device)
{
  struct pi_himax_conf cam_conf;

  pi_himax_conf_init(&cam_conf);

  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;

  return 0;
}

void test_ili9341_with_himax(void)
{
    printf("Entering main controller...\n");

  pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

  imgBuff0 = (unsigned char *)pmsis_l2_malloc((CAM_WIDTH*CAM_HEIGHT)*sizeof(unsigned char));
  if (imgBuff0 == NULL) {
      printf("Failed to allocate Memory for Image \n");
      pmsis_exit(1);
  }

  if (open_display(&ili))
  {
    printf("Failed to open display\n");
    pmsis_exit(-1);
  }

  if (open_camera_himax(&device))
  {
    printf("Failed to open camera\n");
    pmsis_exit(-2);
  }

  buffer.data = imgBuff0+CAM_WIDTH*2+2;
  buffer.stride = 4;

  // WIth Himax, propertly configure the buffer to skip boarder pixels
  pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0+CAM_WIDTH*2+2);
  pi_buffer_set_stride(&buffer, 4);
  pi_buffer_set_format(&buffer, CAM_WIDTH, CAM_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);

  pi_camera_control(&device, PI_CAMERA_CMD_START, 0);
  pi_camera_capture_async(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT, pi_task_callback(&task, cam_handler, &device));


  while (*(int *)&nb_frame != 0)
  {
    pi_yield();
  }

  pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS ili9341 with himax test ***\n\n");
    return pmsis_kickoff((void *) test_ili9341_with_himax);
}
