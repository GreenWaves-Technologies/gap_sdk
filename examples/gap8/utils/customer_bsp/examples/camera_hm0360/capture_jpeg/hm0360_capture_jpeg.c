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

/* constants */
#ifdef IMG_QVGA
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#elif defined IMG_QQVGA
#define CAMERA_WIDTH  ( 160 )
#define CAMERA_HEIGHT ( 120 )
#else 
#define CAMERA_WIDTH  ( 640 )
#define CAMERA_HEIGHT ( 480 )
#endif
//#define CAMERA_HEIGHT ( 100 )
#define CAMERA_PIXEL_SIZE (1)
#define ITER_SIZE  (9600)

/* statics */
static char file_name[30];
static struct pi_device cam;
static struct pi_device ram;
static uint8_t * image;

static int32_t open_camera_hm0360(struct pi_device *device)
{
    struct pi_hm0360_conf cam_conf;
    pi_hm0360_conf_init(&cam_conf);
    #ifdef IMG_QVGA
    cam_conf.format = PI_CAMERA_QVGA;
    #elif defined IMG_QQVGA
    cam_conf.format = PI_CAMERA_QQVGA;
    #else
    cam_conf.format = PI_CAMERA_VGA;
    #endif
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}

static int process_image(int idx)
{

    jpeg_encoder_t enc;
    int pgm_header_size;
    unsigned int width=320, height=240;
    int image_size = width*height;

    // Allocate output jpeg image
    uint8_t *jpeg_image = pi_l2_malloc(20*1024);
    if (jpeg_image == NULL)
        return -1;

    struct jpeg_encoder_conf enc_conf;

    jpeg_encoder_conf_init(&enc_conf);

    #if RUN_ENCODER_ON_FC
    enc_conf.flags=0x0;
    #else
    enc_conf.flags=JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD;
    #endif
    enc_conf.width = CAMERA_WIDTH;
    enc_conf.height = CAMERA_HEIGHT;

    if (jpeg_encoder_open(&enc, &enc_conf))
        return -1;

    if (jpeg_encoder_start(&enc))
        return -1;


    // Get the header so that we can produce full JPEG image
    pi_buffer_t bitstream;
    bitstream.data = jpeg_image;
    bitstream.size = image_size;
    uint32_t header_size, footer_size, body_size;

    if (jpeg_encoder_header(&enc, &bitstream, &header_size))
      return -1;

    //printf("Header size %d\n",header_size);

    // Now get the encoded image
    pi_buffer_t buffer;
    buffer.data = image;
    buffer.size = image_size;
    buffer.width = width;
    buffer.height = height;
    bitstream.data = &jpeg_image[header_size];

    pi_perf_conf(1<<PI_PERF_CYCLES);
    pi_perf_start();
    pi_perf_reset();

    if (jpeg_encoder_process(&enc, &buffer, &bitstream, &body_size))
      return -1;

    pi_perf_stop();

    // Ans finally get the footer
    bitstream.data = &jpeg_image[body_size + header_size];
    if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
      return -1;

    int bitstream_size = body_size + header_size + footer_size;

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
    pi_l2_free(jpeg_image,20*1024);

    printf("Image %s saved to host\n",file_name);
    return 0;

}

static int32_t open_camera(struct pi_device *device)
{
    return open_camera_hm0360(device);
}

static void test_camera_double_buffer(void)
{
    printf("Launching test ...\n");
    int32_t errors = 0;

    /* allocate memory for images */

    image = (uint8_t *) pmsis_l2_malloc(320*240);
    if (image == NULL)
    {
        printf("Failed to allocate Memory for Image\n");
        pmsis_exit(-1);
    }
    

    /* open camera */

    printf("Opening camera ...\n");
    errors = open_camera(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }

#ifdef FRAME_DROP
    unsigned int cfg_glob = (*(volatile unsigned int *)(long)(0x1A1024A0));
    cfg_glob |= ((0x3<<3)|0x1); // enable frame drop, and drop 1 image
    (*(volatile unsigned int *)(long)(0x1A1024A0)) = cfg_glob;
#endif

    int idx =0;
    printf("Starting image capture...\n");
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    while (idx++ < 10)
    {

        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam,image,320*240);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

        printf("capture done\n");

        process_image(idx);

    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera software double buffering ***\n\n");
    return pmsis_kickoff((void *) test_camera_double_buffer);
}
