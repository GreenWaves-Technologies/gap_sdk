/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "kwsInfo.h"
#include "Gap.h"
#include "kwsKernels.h"
#include <limits.h>

#define STACK_SIZE      2048
#define SLAVE_STACK_SIZE 1024
AT_HYPERFLASH_FS_EXT_ADDR_TYPE kws_L3_Flash = 0;

// Softmax always outputs Q15 short int even from 8 bit input
PI_L2 short int *ResOut;
typedef short int KWS_IMAGE_IN_T;
PI_L2 short int* ImageIn;

char labels[][20]={
    "_silence_",
    "_unknown_",
    "yes",
    "no",
    "up",
    "down",
    "left",
    "right",
    "on",
    "off",
    "stop",
    "go"
};

int read_raw_image(char* filename, int16_t* buffer,int w,int h){
    struct pi_fs_conf conf;
    static struct pi_device fs;
    static pi_fs_file_t *file;
    unsigned int ReadSize=0;

    pi_fs_conf_init(&conf);
    conf.type = PI_FS_HOST;
    pi_open_from_conf(&fs, &conf);
    
    if (pi_fs_mount(&fs))
        return -2;

    file = pi_fs_open(&fs, filename, PI_FS_FLAGS_READ);
    if (file == NULL) return -3;


    {
        char *TargetImg = (char*)buffer;
        unsigned int RemainSize = w*h*sizeof(uint16_t);
        
        while (RemainSize > 0)
        {
            unsigned int Chunk = Min(4096, RemainSize);
            unsigned R = pi_fs_read(file,TargetImg, Chunk);
            ReadSize+=R;
            if (R!=Chunk) break;
            TargetImg += Chunk; RemainSize -= Chunk;
        }
    }

    pi_fs_close(file);
    pi_fs_unmount(&fs);
    
    printf("Image %s, [W: %d, H: %d], Gray, Size: %d bytes, Loaded sucessfully\n", filename, w, h, ReadSize);

    return 0;

}


static void Runkws()

{
  printf("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  kwsCNN(ImageIn, ResOut);
  printf("Runner completed\n");


}

void test_kws(void)
{
    char *ImageName = "../../../images/go.dat";

    printf("Entering main controller\n");

    unsigned int Wi = 0, Hi = 0;
    /* Input image size. */
    unsigned int W = 40, H = 98;


    ImageIn = (short int *) pi_l2_malloc(sizeof(KWS_IMAGE_IN_T) * W * H);
    printf("=====>imagein %p\n",ImageIn);
    if (ImageIn == NULL)
    {
        printf("Failed to allocate Memory for Image (%d bytes)\n", sizeof(KWS_IMAGE_IN_T) * W * H);
        pmsis_exit(-1);
    }

    printf("Reading image\n");
    if (read_raw_image(ImageName, ImageIn, W, H)) {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-2);
    }

    printf("Finished reading image\n");

    ResOut = (short int *) pi_l2_malloc(12 * sizeof(short int));
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 10*sizeof(short int));
        pmsis_exit(-3);
    }
    
    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }

    printf("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (kwsCNN_Construct())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-5);
    }
    
    printf("Call cluster\n");
    struct pi_cluster_task task = {0};
    task.entry = Runkws;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    task.slave_stack_size = (unsigned int) SLAVE_STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    kwsCNN_Destruct();
    
    // Close the cluster
    printf("Closing Cluster\n");
    pi_cluster_close(&cluster_dev);

    //Checki Results
    int rec_digit = -1;
    short int highest = SHRT_MIN;

    for(int i = 0; i < 12; i++) {
        //printf("%f ",FIX2FP(ResOut[i],S5_Op_output_1_Q));
        //printf("%d \n",ResOut[i]);
        if(ResOut[i] > highest) {
	       highest = ResOut[i];
	       rec_digit = i;
        }
    }

    printf("Recognized: %s with score: %f\n", labels[rec_digit], FIX2FP(ResOut[rec_digit],kws_Output_1_Q));

    #if defined(PERF)
    {
        unsigned int TotalCycles = 0, TotalOper = 0;
        printf("\n");
        for (unsigned int i=0; i<(sizeof(kwsPerf)/sizeof(unsigned int)); i++)
        {
            printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", kwsLName[i], kwsPerf[i], kwsOperCount[i], ((float) kwsOperCount[i])/ kwsPerf[i]);
            TotalCycles += kwsPerf[i]; TotalOper += kwsOperCount[i];
        }
        printf("\n");
        printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
        printf("\n");
    }
    #endif  /* PERF */
 
    printf("Ended\n");

    int status=-1;
    if (rec_digit==11 && FIX2FP(ResOut[rec_digit],kws_Output_1_Q) > 0.30) status=0;
    //old check was: ResOut[rec_digit]==10926 

    else {printf("Output Error %s %f \n",labels[rec_digit],FIX2FP(ResOut[rec_digit],kws_Output_1_Q));}
    pmsis_exit(status);
}

int main()
{
    printf("\n\n\t *** NNTOOL KWS Example ***\n\n");
    return pmsis_kickoff((void *) test_kws);
}
