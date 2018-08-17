/*
 * Copyright (c) 2017 GreenWaves Technologies SAS
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
#include "rt/rt_api.h"
#include "ImgIO.h"

#define imgW    324
#define imgH    244
#define LL_X    60
#define LL_Y    60
#define UR_X    260
#define UR_Y    180

RT_FC_TINY_DATA rt_camera_t *camera1;
RT_FC_TINY_DATA rt_cam_conf_t *cam1_conf;
RT_FC_TINY_DATA unsigned int  err = 0;

typedef struct {
  unsigned int	L2_Addr;
  unsigned int 	Size;
  unsigned int	imgReady;
  unsigned int	camReady;
} ImgDescriptor;

RT_L2_DATA ImgDescriptor ImageOutHeader;
RT_L2_DATA unsigned char imgBuff0[PPM_HEADER + (imgW*imgH)];

static void CreatePPMHeader(unsigned char *ImgOut, ImgDescriptor *ImgOutHeader, unsigned int W, unsigned int H)
{
  unsigned char Buffer[PPM_HEADER];
  unsigned int Ind = 0, x, i, L;

  /* P5<cr>* */
  Buffer[Ind++] = 0x50; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  /* W <space> */
  x = W; L=0;
  while (x>0) { x = x/10; L++; }
  x = W; i = 1;
  while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  Ind += L;
  Buffer[Ind++] = 0x20;

  /* H <cr> */
  x = H; L=0;
  while (x>0) { x = x/10; L++; }
  x = H; i = 1;
  while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  Ind += L;
  Buffer[Ind++] = 0xA;

  /* 255 <cr> */
  Buffer[Ind++] = 0x32; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  for (i=0; i<Ind; i++) ImgOut[-Ind+i] = Buffer[i];

  /* Output image code as a ppm mono address in L2 */
  ImgOutHeader->L2_Addr = ((unsigned int) ImgOut) - Ind;
  /* Size of the ppm image including header */
  ImgOutHeader->Size = W*H+Ind;
  /* Image is ready */
  ImgOutHeader->imgReady = 1;
}

static void cam_param_conf(rt_cam_conf_t *conf){
  conf->resolution = QVGA;
  conf->format = HIMAX_MONO_COLOR;
  conf->fps = fps30;
  conf->slice_en = DISABLE;
  conf->shift = 0;
  conf->frameDrop_en = DISABLE;
  conf->frameDrop_value = 0;
  conf->cpiCfg = UDMA_CHANNEL_CFG_SIZE_16;
}

void testHimax_Full(){

  //Enable Camera Interface
  rt_cam_control(camera1, CMD_START, 0); 
  // Get an event from the free list, which can then be used for a blocking wait using rt_event_wait.
  rt_event_t *event = rt_event_get_blocking(NULL);
  // Programme the camera capture job to udma
  rt_camera_capture (camera1, (imgBuff0 + PPM_HEADER + 2), imgW*imgH, event);
  // Start to receive the frame.
  // Wait until the capture finished
  rt_event_wait(event);
  // Stop the camera, camera interface is clock gated.
  rt_cam_control(camera1, CMD_PAUSE, 0);
}

int main()
{
  printf("Entering main controller\n");
  // prepare 1 event for blocking usage.
  if (rt_event_alloc(NULL, 1)) return -1;

  char imgName[50];

  // Initialize the parameter of camera.
  cam1_conf = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_cam_conf_t));
  cam_param_conf(cam1_conf);
  // open the camera, the conf structure will be stored in camera object
  camera1 = rt_camera_open("camera", cam1_conf, 0);
  if (camera1 == NULL) return -1;
  // free the memory of camera configuration structure
  rt_free(RT_ALLOC_FC_DATA, cam1_conf, sizeof(rt_cam_conf_t));

  rt_bridge_connect(NULL);

  //Init Camera Interface
  rt_cam_control(camera1, CMD_INIT, 0);
  rt_time_wait_us(1000000); //Wait camera calibration

  printf("ready to take a photo\n");
  for (int i=0; i<10; i++){
      printf("taking picture\n");

      // Take a full image and check it
      testHimax_Full();
      printf("Picture taken!\n");

      ImageOutHeader.camReady = 0;
      //CreatePPMHeader((imgBuff0+PPM_HEADER), &ImageOutHeader, imgW, imgH);
      sprintf(imgName, "../../../imgOut/img_OUT%d.ppm", i);
      printf("imgName: %s\n", imgName);
      WriteImageToFile(imgName,imgW,imgH,(imgBuff0 + PPM_HEADER));
  }
  // Close the camera
  rt_camera_close(camera1, 0);
  printf("Test success: end of the test\n");

  return err;
}
