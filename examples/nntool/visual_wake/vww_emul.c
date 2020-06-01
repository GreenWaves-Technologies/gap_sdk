/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "vww_emul.h"
#include "vwwKernels.h"
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s


#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

#ifndef STACK_SIZE
#define STACK_SIZE     2048 
#endif

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>

// Softmax always outputs Q15 short int even from 8 bit input
signed short int Output_1[2];
typedef signed char IMAGE_IN_T;
unsigned char Input_1[AT_INPUT_SIZE];

static void RunNetwork()
{
  printf("Running on cluster Input_1(%p) Output_1(%p)\n", Input_1, Output_1);
  __PREFIX(CNN)(Input_1, Output_1);
  printf("Runner completed Input_1(%p) Output_1(%p)\n", Input_1, Output_1);

  printf("\n");
  
  //Checki Results
  if (Output_1[1] > Output_1[0]) {
    printf("person seen (%d, %d)\n", Output_1[0], Output_1[1]);
  } else {
    printf("no person seen (%d, %d)\n", Output_1[0], Output_1[1]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) 
{
  if (argc < 2) {
    printf("Usage: %s [image_file]\n", argv[0]);
    exit(1);
  }
  char *ImageName = argv[1];

  //Input image size

  printf("Entering main controller\n");

  printf("Constructor\n");

  // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
  if (__PREFIX(CNN_Construct)())
  {
    printf("Graph constructor exited with an error\n");
    return 1;
  }

  printf("Reading image Input_1(%p)\n", Input_1);
  //Reading Image from Bridge
  if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Input_1, AT_INPUT_SIZE*sizeof(IMAGE_IN_T), IMGIO_OUTPUT_CHAR, 0)) {
    printf("Failed to load image %s\n", ImageName);
    return 1;
  }
  printf("Finished reading image\n");

  printf("Call cluster Input_1(%p)\n", Input_1);
  // Execute the function "RunNetwork" on the cluster.
  RunNetwork(NULL);
  
  __PREFIX(CNN_Destruct)();

  printf("Ended\n");
  return 0;
}

