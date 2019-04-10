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
#include "ImgIO.h"

char buffer[8192];

int main()
{
  unsigned int width = 322, height = 242;

  if (rt_event_alloc(NULL, 8)) return -1;

  printf("Connecting to bridge...\n");
  //Open Debug bridge connection for FILE IO
  rt_bridge_connect(1, NULL);
  printf("Connection done.\n");

  unsigned char *InBuffer = (unsigned char *) rt_alloc( RT_ALLOC_L2_CL_DATA, width*height*sizeof(unsigned char));

  ReadImageFromFile("../../../Pedestrian.ppm",&width,&height,InBuffer,width*height*sizeof(unsigned char));
  WriteImageToFile("../../../Pedestrian_OUT.ppm",width,height,InBuffer);

  rt_bridge_disconnect(NULL);
  printf("Test success\n");

  return 0;
}
