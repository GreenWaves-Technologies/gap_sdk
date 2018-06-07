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

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

/*
 * This example shows how to wait for a specific amount of time.
 */

#include <stdio.h>
#include "rt/rt_api.h"

#define WAIT_TIME 1200

int main()
{
  unsigned long long start_time, end_time;

  start_time = rt_time_get_us();

  // The time we specify here is in microseconds and is a minimum amount of time
  // where the core will sleep. Due to timer resolution, it may wait a bit more.
  rt_time_wait_us(WAIT_TIME);

  end_time = rt_time_get_us();

  printf("Start time %lld\n", start_time);
  printf("End time %lld\n", end_time);
  printf("Test success\n");

  return 0;
}
