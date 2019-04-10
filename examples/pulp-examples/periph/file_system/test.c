/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "rt/rt_api.h"
#include "stdio.h"

#define ITER_SIZE 1024
#define COUNT 10

static char buff[2][ITER_SIZE+1];
static int count_enqueued = 0;
static int count_done = 0;
static rt_file_t *file;
static int curr_pos = 0;

static void end_of_rx(void *arg);

static void check_file(int buff_index)
{
  if (count_enqueued < COUNT)
  {
    do {
      int size = rt_fs_read(file, buff[buff_index], ITER_SIZE, rt_event_get(NULL, end_of_rx, (void *)buff_index));
      if (size < ITER_SIZE) {
        rt_fs_seek(file, 0);
      }
      buff[buff_index][size] = 0;
      if (size != 0) {
        count_enqueued++;
        break;
      }
    } while(1);
  }
}

static void end_of_rx(void *arg)
{
  int index = (int)arg;
  //printf("End of RX for id %d\n", index);
  printf(buff[index]);
  count_done++;
  check_file(index);
}

static void handle_error()
{
  int error = rt_error_current();
  printf("Caught error (error code 0x%x): %s\n", error, rt_error_str(error));
  exit(-1);
}

static void handle_async_error(void *arg, rt_event_t *event, int error, void *object)
{
  printf("Received error (error code 0x%x, event %p, object: %p): %s\n", error, event, object, rt_error_str(error));
  exit(-1);
}

int main()
{
  printf("Entering main controller\n");

  rt_error_conf(NULL, handle_async_error, NULL);

  if (rt_event_alloc(NULL, 16)) return -1;

  rt_padframe_profile_t *profile_hyper = rt_pad_profile_get("hyper");
  if (profile_hyper == NULL) {
      printf("pad config error\n");
      return 1;
  }
  rt_padframe_set(profile_hyper);
  printf ("pad config done\n");

  rt_fs_conf_t conf;
  rt_fs_conf_init(&conf);

  rt_fs_t *fs = rt_fs_mount("hyperflash", &conf, NULL);
  if (fs == NULL) handle_error();
  printf("Hyperflash fs mounted\n");

  file = rt_fs_open(fs, "hello.txt", 0, NULL);
  if (file == NULL){
      printf("file open failed\n");
      return -1;
  }
  printf("file open success\n");

  check_file(0);
  //check_file(1);

  while(count_done != COUNT) {
    rt_event_execute(NULL, 1);
  }

  printf("\n");

  rt_fs_unmount(fs, NULL);
  printf("fs unmounted\n");
  printf ("Test success\n");

  return 0;
}
