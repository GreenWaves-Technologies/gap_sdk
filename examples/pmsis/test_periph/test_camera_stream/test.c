/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

// This example shows how to strean the capture of a camera image using
// small buffers.
// This is using aynchronous transfers to make sure a second buffer is always ready
// when the current is finished, to not lose any data.

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/himax.h"
#include <bsp/fs/hostfs.h>

#define WIDTH    324
#define HEIGHT   244

#define BUFF_SIZE (WIDTH*HEIGHT)
#define ITER_SIZE  (64*1024)

PI_L2 unsigned char *buff;

static struct pi_device camera;
static pi_task_t rx_tasks[2];
static int remaining_size;
static volatile int done;
static int nb_transfers;
static unsigned char *current_buff;
static int current_task;



static void handle_transfer_end(void *arg);


// This is called to enqueue new transfers
static void enqueue_transfer()
{
  // We can enqueue new transfers if there are still a part of the image to
  // capture and less than 2 transfers are pending (the dma supports up to 2 transfers
  // at the same time)
  while (remaining_size > 0 && nb_transfers < 2)
  {
    int iter_size = ITER_SIZE;
    if (remaining_size < iter_size)
      iter_size = remaining_size;

    pi_task_t *task = &rx_tasks[current_task];

    // Enqueue a transfer. The callback will be called once the transfer is finished
    // so that  a new one is enqueued while another one is already running
    pi_camera_capture_async(&camera, current_buff, iter_size, pi_task_callback(task, handle_transfer_end, NULL));

    remaining_size -= iter_size;
    nb_transfers++;
    current_buff += iter_size;
    current_task ^= 1;
  }

}


static void handle_transfer_end(void *arg)
{
  nb_transfers--;

  enqueue_transfer();

  if (nb_transfers == 0)
    done = 1;
}


static int open_camera(struct pi_device *device)
{
  printf("Opening Himax camera\n");
  struct pi_himax_conf cam_conf;
  pi_himax_conf_init(&cam_conf);

  cam_conf.format = PI_CAMERA_QVGA;

  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;

  return 0;
}


static int test_entry()
{
  printf("Entering main controller\n");

  if (open_camera(&camera))
  {
    printf("Failed to open camera\n");
    goto error;
  }

  buff = pmsis_l2_malloc(WIDTH*HEIGHT);
  if (buff == NULL) goto error;

  remaining_size = BUFF_SIZE;
  nb_transfers = 0;
  current_buff = buff;
  current_task = 0;
  done = 0;

  enqueue_transfer();

  pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);

  while(!done)
  {
    pi_yield();
  }

  pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
  pi_camera_close(&camera);

  struct pi_hostfs_conf conf;
  pi_hostfs_conf_init(&conf);

  struct pi_device fs;

  pi_open_from_conf(&fs, &conf);

  if (pi_fs_mount(&fs))
    return -1;

  pi_fs_file_t *file = pi_fs_open(&fs, "output.raw", PI_FS_FLAGS_WRITE);
  if (file == NULL) return -3;

  pi_fs_write(file, buff, BUFF_SIZE);

  pi_fs_close(file);


  printf("Test success\n");

  return 0;

error:
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
