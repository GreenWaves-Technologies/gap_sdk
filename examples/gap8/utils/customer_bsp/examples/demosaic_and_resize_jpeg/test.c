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

static int process_image(unsigned char *image, char* file_name, int idx, int width, int height, int channels)
{
    jpeg_encoder_t enc;
    int pgm_header_size;
    int image_size = width*height;

    // Allocate output jpeg image
    uint8_t *jpeg_image = pi_l2_malloc(60*1024);
    if (jpeg_image == NULL)
        return -1;

    struct jpeg_encoder_conf enc_conf;

    jpeg_encoder_conf_init(&enc_conf);

    #if RUN_ENCODER_ON_FC
    enc_conf.flags=0x0;
    #else
    enc_conf.flags=JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD;
    #endif
    
    enc_conf.flags |= JPEG_ENCODER_FLAGS_COLOR;

    enc_conf.width = width;
    enc_conf.height = height;

    if (jpeg_encoder_open(&enc, &enc_conf))
        return -1;

    if (jpeg_encoder_start(&enc))
        return -1;


    // Get the header so that we can produce full JPEG image
    pi_buffer_t bitstream;
    bitstream.data = jpeg_image;
    bitstream.size = image_size;
    
    uint32_t header_size, footer_size, body_size, body_size_sc,body_size_ec;
    uint32_t written_size;
    if (jpeg_encoder_header(&enc, &bitstream, &header_size))
      return -1;

    written_size=header_size;

    pi_buffer_t buffer;
    buffer.data = image;
    buffer.size = image_size;
    buffer.width = width;
    buffer.height = height;
    bitstream.data = &jpeg_image[written_size];


    if (jpeg_encoder_process(&enc, &buffer, &bitstream, &body_size))
      return -1;
    written_size+=body_size;
    bitstream.data = &jpeg_image[written_size];
    
    //printf("Header size %d\n",header_size);

    // And finally get the footer
    if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
      return -1;
    written_size+=footer_size;
    int bitstream_size = written_size;

    printf("Jpeg Encoding done - size %d - cycles %d\n",bitstream_size,pi_perf_read(PI_PERF_CYCLES));

    jpeg_encoder_stop(&enc);
    //The stop is closing the cluster, which we do not want to do it
    //TODO add a flag to leave the cluster open
    //jpeg_encoder_close(&enc);

    // Now flush the image to the workstation using semi-hosting

    struct pi_fs_conf host_fs_conf;
    pi_fs_conf_init(&host_fs_conf);
    struct pi_device host_fs;

    host_fs_conf.type = PI_FS_HOST;

    pi_open_from_conf(&host_fs, &host_fs_conf);

    if (pi_fs_mount(&host_fs)){
        printf("Error mounting FS\n");
      return -1;
    }
    sprintf(file_name,"../../../imgTest%02d.jpg",idx);
    void *File = pi_fs_open(&host_fs, file_name, PI_FS_FLAGS_WRITE);

    pi_fs_write(File, jpeg_image, bitstream_size);
    pi_l2_free(jpeg_image,60*1024);

    printf("Image %s saved to host\n",file_name);
    return 0;

}

#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
#define FIXQ 11


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

    process_image(Output_1, "../../../jpeg_output", 1, 160, 120, 3);
    
    pmsis_l2_malloc_free(Output_1 , IMG_OUT_SIZE);
    pmsis_l2_malloc_free(Input_1  , IMG_W*IMG_H);

   
    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera software double buffering ***\n\n");
    return pmsis_kickoff((void *) test);
}
