/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "pmsis/device.h"
#include "pmsis/rtos/pmsis_os.h"
#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"
#include "pmsis/rtos/os_frontend_api/pmsis_task.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#if defined(CONFIG_HIMAX)
#include "bsp/camera/himax.h"
#elif defined(CONFIG_MT9V034)
#include "bsp/camera/mt9v034.h"
#endif

#if defined(CONFIG_HIMAX)
#define WIDTH    324
#define HEIGHT   244
#else
#define WIDTH    320
#define HEIGHT   240
#endif

#define BUFF_SIZE (WIDTH*HEIGHT)

PI_L2 unsigned char *buff[2];

static int open_camera(struct pi_device *device)
{
#if defined(CONFIG_HIMAX)
  printf("Opening Himax camera\n");
  struct himax_conf cam_conf;
  himax_conf_init(&cam_conf);
#elif defined(CONFIG_MT9V034)
  printf("Opening mt9v034 camera\n");
  struct mt9v034_conf cam_conf;
  mt9v034_conf_init(&cam_conf);
#endif

  cam_conf.format = CAMERA_QVGA;

  pi_open_from_conf(device, &cam_conf);
  if (camera_open(device))
    return -1;

  return 0;
}

static int test_entry()
{
  struct pi_device camera;
  pi_task_t task;

  printf("Entering main controller\n");

  if (open_camera(&camera))
  {
    printf("Failed to open camera\n");
    goto error;
  }

  buff[0] = pmsis_l2_malloc(WIDTH*HEIGHT);
  if (buff[0] == NULL) goto error;

  buff[1] = pmsis_l2_malloc(WIDTH*HEIGHT);
  if (buff[1] == NULL) goto error;

  camera_capture_async(&camera, buff[0], WIDTH*HEIGHT, pi_task_block(&task));

  camera_control(&camera, CAMERA_CMD_START, 0);

  pi_task_wait_on(&task);

  camera_control(&camera, CAMERA_CMD_STOP, 0);

  for (int j=0; j<1; j++)
  {
    for (int i=0; i<BUFF_SIZE; i++)
    {
      unsigned char expected;
      if (j == 0)
        expected = i & 0x7f;
      else
        expected = i | 0x80;
      if (expected != buff[j][i])
      {
        printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, buff[j][i]);
        goto error;
      }
    }
  }

  camera_close(&camera);
  printf("Test success\n");

  return 0;

error:
  printf("Test failure\n");
  return -1;
}

static void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
