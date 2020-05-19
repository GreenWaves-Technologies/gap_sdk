# pylint: disable=line-too-long, pointless-string-statement, unused-variable

from .templet import stringfunction
import sys
import subprocess
import os

@stringfunction
# pylint: disable=unused-argument
def generator_eval_code(project_name, dataset, in_size):
	'''
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
*/

#include <stdio.h>

#include "Gap.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#include "helpers.h"

#include "${project_name}Kernels.h"
#include "ImgIO.h"
#include <dirent.h>

#define AT_HYPERFLASH_FS_EXT_ADDR_TYPE ${project_name}_L3_Flash 0
#define AT_INPUT_WIDTH	${in_size[0]}
#define AT_INPUT_HEIGHT	${in_size[1]}
#define AT_INPUT_COLORS	${in_size[2]}
#define AT_INPUT_SIZE	(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)
#ifndef STACK_SIZE
#define STACK_SIZE		2048
#endif

// Softmax always outputs Q15 short int even from 8 bit input
L2_MEM short int *ResOut;
typedef signed char IMAGE_IN_T;
L2_MEM IMAGE_IN_T *ImageIn;
#define MAXCHAR 1000
#define NUM_CLASSES ${dataset.num_classes}
#define DATASET_DIM ${dataset.num_samples}

static int RunNetwork()
{
  ${project_name}CNN(ResOut);
  //Checki Results
  int max = 0;
  int index = 0;
  for(int i=0; i<NUM_CLASSES; i++){
	if (ResOut[i] > max){
		max=ResOut[i];
		index = i;
	} 
  }
  return index;
}

int main(int argc, char *argv[]) 
{
  if (argc < 2) {
    printf("Usage: %s [dataset_folder]\\n", argv[0]);
    exit(1);
  }
  struct dirent *dp;
  DIR *dfd;

  char *dir = argv[1];
  printf("Entering main controller\\n");
  printf("Constructor\\n");
  if (${project_name}CNN_Construct())
  {
    printf("Graph constructor exited with an error\\n");
    return 1;
  }

  if ((dfd = opendir(dir)) == NULL)
  {
    fprintf(stderr, "Can't open %s\\n", dir);
    return 0;
  }
  char filename_qfd[100];
  char new_name_qfd[100];
  int labels[DATASET_DIM];
  int i=0;
  int predicted=0;
  int result;

  while ((dp = readdir(dfd)) != NULL)
  {
    struct stat stbuf ;
    sprintf(filename_qfd , "%s/%s" ,dir,dp->d_name);

    if(stat(filename_qfd,&stbuf) == -1)
    {
     printf("Unable to stat file: %s\\n",filename_qfd) ;
     continue ;
    }

    if ( ( stbuf.st_mode & S_IFMT ) == S_IFDIR )
    {
    	continue;
    	// Skip directories
    } else {
    	//reading labels from filenames
      	labels[i] = dp->d_name[strlen(dp->d_name)-5] - '0'; 
      	//Reading Image from Bridge
      	if (ReadImageFromFile(filename_qfd, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, (char *)$(project_name)_L2_Memory, AT_INPUT_SIZE*sizeof(IMAGE_IN_T), 1, 0)) 
      	{
        	printf("Failed to load image %s\\n", filename_qfd);
        	return 1;
      	}	
		ResOut = (short int *) AT_L2_ALLOC(0, NUM_CLASS*sizeof(short int));
		if (ResOut==0) {
		printf("Failed to allocate Memory for Result (%ld bytes)\\n", NUM_CLASS*sizeof(short int));
		return 1;
		}
		// Execute the function "RunNetwork"
		result = RunNetwork(NULL);
		if (result==labels[i]){
		predicted++;
		}
    }
    i++;
  }
  printf("accuracy rate = %.2f%%\\n", 100*(float)predicted/(float)i);  
  $(project_name)CNN_Destruct();
  printf("Ended\\n");
  return 0;
}

'''

def write_test_main_template(G, test_dataset, in_size, filepath):
	if not os.path.exists(filepath):
		f = open(filepath, 'w')
		f.write(generator_eval_code(G.name, test_dataset, in_size))
		f.close()
	return


def num_elements(path, num_el):
    for f in os.scandir(path):
        num_el += 1
        if f.is_dir():
            num_elements(f, num_el)
    return num_el

class test_dataset_4_eval():
	def __init__(self, folder_path, num_classes=None):
	    self.folder_path = folder_path
	    if num_classes is not None:
	        self.num_classes = num_classes
	    else: #default: one per subdir
	        self.num_classes = len([f.path for f in os.scandir(self.folder_path) if f.is_dir()])
	    self.num_samples = num_elements(self.folder_path, 0)
	    print(self.num_classes, self.num_samples, self.folder_path)
	