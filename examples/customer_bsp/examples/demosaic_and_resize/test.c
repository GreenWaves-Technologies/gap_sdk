#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "gapoc_c.h"
#include "bsp/camera.h"

#include "bsp/ram.h"
#include "bsp/ram/spiram.h"

/* gaplib includes */
#include "gaplib/ImgIO.h"
#include "gaplib/jpeg_encoder.h"

#include "DeMosaicKernels.h"

/* constants */

#ifdef IMG_VGA

#define IMG_W 640
#define IMG_H 480
#define IMG_C 3
#define IMG_SIZE (IMG_W*IMG_H*IMG_C)

#define IMG_OUT_W IMG_W/2
#define IMG_OUT_H IMG_H/2
#define IMG_OUT_C 3
#define IMG_OUT_SIZE (IMG_OUT_W*IMG_OUT_H*IMG_OUT_C)


#elif defined IMG_QVGA

#define IMG_W 320
#define IMG_H 240
#define IMG_C 3
#define IMG_SIZE (IMG_W*IMG_H*IMG_C)

#define IMG_OUT_W IMG_W/2
#define IMG_OUT_H IMG_H/2
#define IMG_OUT_C 3
#define IMG_OUT_SIZE (IMG_OUT_W*IMG_OUT_H*IMG_OUT_C)

#else
#error Input Size not Defined
#endif
unsigned char *Input_1;
unsigned char *Output_1;


void demosaic_cluster(){
    printf("Enterin Cluster...\n");
    DeMosaic_L1_Memory = pmsis_l1_malloc(_DeMosaic_L1_Memory_SIZE);

    DeMosaicing(Input_1,Output_1);
    
    pmsis_l1_malloc_free(DeMosaic_L1_Memory,_DeMosaic_L1_Memory_SIZE);

    printf("Quittin Cluster...\n");
}

static void test(void)
{
    printf("Launching test ...\n");

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

    Input_1  = pmsis_l2_malloc(IMG_W*IMG_H);
    Output_1 = pmsis_l2_malloc(IMG_OUT_SIZE);

    /*--------------------------OPEN RAW BAYER IMAGE------------------------------*/
    if (ReadImageFromFile("../../../samples/imgTest01_bayer.pgm", IMG_W, IMG_H, 1, Input_1, IMG_W*IMG_W*1, IMGIO_OUTPUT_CHAR, 0)) {
    printf("Failed to load image...\n");
    pmsis_exit(-1);
    }

    /*--------------------------CLUSTER OPEN------------------------------*/
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    pi_cluster_conf_init(&conf);
    pi_open_from_conf(&cluster_dev, (void *)&conf);
    pi_cluster_open(&cluster_dev);
    

    /*--------------------------TASK SETUP------------------------------*/
    struct pi_cluster_task *task_cluster = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    if(task_cluster==NULL) {
        printf("pi_cluster_task alloc Error!\n");
        pmsis_exit(-1);
    }
    printf("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
    memset(task_cluster, 0, sizeof(struct pi_cluster_task));
    task_cluster->entry = &demosaic_cluster;
    task_cluster->stack_size = STACK_SIZE;
    task_cluster->slave_stack_size = SLAVE_STACK_SIZE;
    task_cluster->arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, task_cluster);

    pi_cluster_close(&cluster_dev);
    pmsis_l2_malloc_free(task_cluster,sizeof(struct pi_cluster_task));
    
    WriteImageToFile("../../../test_out_color.ppm", IMG_OUT_W, IMG_OUT_H, IMG_OUT_C, Output_1, RGB888_IO);    
    
    pmsis_l2_malloc_free(Output_1 , IMG_OUT_SIZE);
    pmsis_l2_malloc_free(Input_1  , IMG_W*IMG_H);

   
    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera software double buffering ***\n\n");
    return pmsis_kickoff((void *) test);
}
