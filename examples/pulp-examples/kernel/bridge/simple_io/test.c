/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
#include "rt/rt_api.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buffer[8192];

int main()
{

  if (rt_event_alloc(NULL, 8)) return -1;

  printf("Connecting to bridge\n");
  rt_bridge_connect(1, NULL);
  printf("Connection done\n");
  int file = rt_bridge_open("../../../test.c", 0, 0, NULL);
  int file2 = rt_bridge_open("../../../out_test.c", O_RDWR | O_CREAT, S_IRWXU, NULL);
  if (file == -1 || file2 == -1) return -1;

  while(1)
  {
    int res = rt_bridge_read(file, buffer, 8192, NULL);
    if (res == 0) break;
    rt_bridge_write(file2, buffer, res, NULL);
    buffer[res] = 0;
    puts(buffer);
  }

  rt_bridge_disconnect(NULL);
  printf("Test success\n");

  return 0;
}
