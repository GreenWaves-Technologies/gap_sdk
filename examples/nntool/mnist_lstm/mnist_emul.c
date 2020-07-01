/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Autotiler includes. */
#include "mnist.h"
#include "mnistKernels.h"
#include "gaplib/ImgIO.h"


#define pmsis_exit(n) exit(n)

#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif

AT_HYPERFLASH_FS_EXT_ADDR_TYPE mnist_L3_Flash = 0;

// Softmax always outputs Q15 short int even from 8 bit input
L2_MEM short int *ResOut;
//Image in is unsigned but the model is trained with -1:1 inputs
L2_MEM unsigned char *Img_In;

#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)
#define AT_INPUT_SIZE_BYTES (AT_INPUT_SIZE*sizeof(char))
//#define PRINT_IMAGE

char *ImageName = NULL;


static void cluster()
{
  mnistCNN(Img_In, 1, ResOut);
  printf("Runner completed\n");

  //Checki Results
  int rec_digit = 0;
  short int highest = ResOut[0];
  for(int i = 0; i < 10; i++) {
    printf("class %d: %d \n", i, ResOut[i]);
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
  }
  printf("\n");

  printf("Recognized: %d\n", rec_digit);
}

int test_mnist(void)
{
    printf("Entering main controller\n");
    printf("Reading image\n");
    //Reading Image from Bridge
    /*------------------- Allocate Image Buffer ------------------------*/
    printf("Going to alloc the image buffer!\n");
    Img_In = (unsigned char *) AT_L2_ALLOC(0, AT_INPUT_SIZE_BYTES);
    if(Img_In==NULL) {
      printf("Image buffer alloc Error!\n");
      pmsis_exit(-1);
    } 

    if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Img_In, AT_INPUT_SIZE_BYTES, IMGIO_OUTPUT_CHAR, 0))
    {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-2);
    }

	#if defined(PRINT_IMAGE)
	    for (int i=0; i<AT_INPUT_HEIGHT; i++)
	    {
	        for (int j=0; j<AT_INPUT_WIDTH; j++)
	        {
	            printf("%03d, ", Img_In[AT_INPUT_WIDTH*i + j]);
	        }
	        printf("\n");
	    }
	#endif  /* PRINT_IMAGE */

    ResOut = (short int *) AT_L2_ALLOC(0, 10 * sizeof(short int));
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 10*sizeof(short int));
        pmsis_exit(-3);
    }

    printf("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (mnistCNN_Construct())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-5);
    }

    printf("Call cluster\n");
    cluster();

    mnistCNN_Destruct();

    AT_L2_FREE(0, Img_In, AT_INPUT_SIZE_BYTES);
    AT_L2_FREE(0, ResOut, 10 * sizeof(short int));
    printf("Ended\n");

    pmsis_exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: mnist [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    printf("\n\n\t *** NNTOOL Mnist Example ***\n\n");
    test_mnist();
    return 0;
}
