/****************************************************************************/
#include "stdio.h"

/* PMSIS includes */
#include "pmsis.h"

/* PMSIS BSP includes */
#if defined(GAPOC)
#include "bsp/gapoc_a.h"
#else
#include "bsp/gapuino.h"
#endif  /* GAPOC */
#if defined(HIMAX)
#include "bsp/camera/himax.h"
#else
#include "bsp/camera/mt9v034.h"
#endif  /* HIMAX */
#if defined(USE_DISPLAY)
#include "bsp/display/ili9341.h"
#endif  /* USE_DISPLAY */

#include "faceDet.h"
#include "FaceDetKernels.h"
#include "ImageDraw.h"
#include "setup.h"

#if defined(HIMAX)
#define CAM_WIDTH    324
#define CAM_HEIGHT   244
#else
#define CAM_WIDTH    320
#define CAM_HEIGHT   240
#endif  /* HIMAX */

#define LCD_WIDTH    320
#define LCD_HEIGHT   240

//#define DEBUG_PRINTF(...)
#define DEBUG_PRINTF printf

static unsigned char *imgBuff0;
static struct pi_device ili;
static pi_buffer_t buffer;
static pi_buffer_t buffer_out;
static struct pi_device cam;

L2_MEM unsigned char *ImageOut;
L2_MEM unsigned int *ImageIntegral;
L2_MEM unsigned int *SquaredImageIntegral;
L2_MEM char str_to_lcd[100];

struct pi_device cluster_dev;
struct pi_cluster_task *task;
struct pi_cluster_conf conf;
ArgCluster_T ClusterCall;

#if defined(USE_DISPLAY)
void setCursor(struct pi_device *device,signed short x, signed short y);
void writeFillRect(struct pi_device *device, unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color);
void writeText(struct pi_device *device,char* str,int fontsize);
#endif  /* USE_DISPLAY */

static int open_display(struct pi_device *device)
{
#if defined(USE_DISPLAY)
    struct pi_ili9341_conf ili_conf;

    pi_ili9341_conf_init(&ili_conf);

    pi_open_from_conf(device, &ili_conf);

    if (pi_display_open(device))
    {
        return -1;
    }
#endif
    return 0;
}

#if defined(USE_CAMERA)
#if defined(HIMAX)
static int open_camera_himax(struct pi_device *device)
{
  struct pi_himax_conf cam_conf;

  pi_himax_conf_init(&cam_conf);

  cam_conf.format = PI_CAMERA_QVGA;

  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;

  return 0;
}
#else
static int open_camera_mt9v034(struct pi_device *device)
{
  struct pi_mt9v034_conf cam_conf;

  pi_mt9v034_conf_init(&cam_conf);
  cam_conf.format = CAMERA_QVGA;

  pi_open_from_conf(device, &cam_conf);
  if (pi_camera_open(device))
    return -1;

  return 0;
}
#endif  /* HIMAX */
#endif  /* USE_CAMERA */

static int open_camera(struct pi_device *device)
{
    #if defined(USE_CAMERA)
    #if defined(HIMAX)
    return open_camera_himax(device);
    #else
    return open_camera_mt9v034(device);
    #endif  /* HIMAX */
    #else
    return 0;
    #endif  /* USE_CAMERA */
}

void test_facedetection(void)
{
    DEBUG_PRINTF("Entering main controller...\n");

    unsigned int W = CAM_WIDTH, H = CAM_HEIGHT;
    unsigned int Wout = 64, Hout = 48;
    unsigned int ImgSize = W*H;

    imgBuff0 = (unsigned char *)pmsis_l2_malloc((CAM_WIDTH*CAM_HEIGHT)*sizeof(unsigned char));
    if (imgBuff0 == NULL)
    {
        DEBUG_PRINTF("Failed to allocate Memory for Image \n");
        pmsis_exit(-1);
    }

    //This can be moved in init
    ImageOut             = (unsigned char *) pmsis_l2_malloc((Wout*Hout)*sizeof(unsigned char));
    ImageIntegral        = (unsigned int *)  pmsis_l2_malloc((Wout*Hout)*sizeof(unsigned int));
    SquaredImageIntegral = (unsigned int *)  pmsis_l2_malloc((Wout*Hout)*sizeof(unsigned int));

    if (ImageOut == 0)
    {
        DEBUG_PRINTF("Failed to allocate Memory for Image (%d bytes)\n", ImgSize*sizeof(unsigned char));
        pmsis_exit(-2);
    }
    if ((ImageIntegral == 0) || (SquaredImageIntegral == 0))
    {
        DEBUG_PRINTF("Failed to allocate Memory for one or both Integral Images (%d bytes)\n", ImgSize*sizeof(unsigned int));
        pmsis_exit(-3);
  }
    DEBUG_PRINTF("malloc done\n");

    board_init();

    if (open_display(&ili))
    {
        DEBUG_PRINTF("Failed to open display\n");
        pmsis_exit(-4);
    }
    DEBUG_PRINTF("display done\n");

    if (open_camera(&cam))
    {
        DEBUG_PRINTF("Failed to open camera\n");
        pmsis_exit(-5);
    }
    DEBUG_PRINTF("Camera open success\n");

    #if defined(HIMAX)
    buffer.data = imgBuff0+CAM_WIDTH*2+2;
    buffer.stride = 4;

    // WIth Himax, propertly configure the buffer to skip boarder pixels
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0+CAM_WIDTH*2+2);
    pi_buffer_set_stride(&buffer, 4);
    #else
    buffer.data = imgBuff0;
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0);
    #endif  /* HIMAX */

    #if defined(USE_DISPLAY)
    buffer_out.data = ImageOut;
    buffer_out.stride = 0;
    pi_buffer_init(&buffer_out, PI_BUFFER_TYPE_L2, ImageOut);
    pi_buffer_set_stride(&buffer_out, 0);
    #endif /* USE_DISPLAY */

    pi_buffer_set_format(&buffer, CAM_WIDTH, CAM_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);

    ClusterCall.ImageIn              = imgBuff0;
    ClusterCall.Win                  = W;
    ClusterCall.Hin                  = H;
    ClusterCall.Wout                 = Wout;
    ClusterCall.Hout                 = Hout;
    ClusterCall.ImageOut             = ImageOut;
    ClusterCall.ImageIntegral        = ImageIntegral;
    ClusterCall.SquaredImageIntegral = SquaredImageIntegral;

    pi_cluster_conf_init(&conf);
    pi_open_from_conf(&cluster_dev, (void*)&conf);
    pi_cluster_open(&cluster_dev);

    task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = faceDet_cluster_init;
    task->arg = &ClusterCall;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

    task->entry = faceDet_cluster_main;
    task->arg = &ClusterCall;

    #if defined(USE_DISPLAY)
    //Setting Screen background to white
    writeFillRect(&ili, 0, 0, 240, 320, 0xFFFF);
    setCursor(&ili, 0, 0);
    writeText(&ili,"      Greenwaves \n       Technologies", 2);
    #endif  /* USE_DISPLAY */
    DEBUG_PRINTF("main loop start\n");

    while (1)
    {
        #if defined(USE_CAMERA)
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAM_WIDTH*CAM_HEIGHT);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #endif  /* USE_CAMERA */

        pi_cluster_send_task_to_cl(&cluster_dev, task);
        DEBUG_PRINTF("end of face detection, num_reponse: %d\n", ClusterCall.num_reponse);

        #if defined(USE_DISPLAY)
        pi_display_write(&ili, &buffer_out, 40, 40, 160, 120);
        if (ClusterCall.num_reponse)
        {
            sprintf(str_to_lcd, "Face detected: %d\n", ClusterCall.num_reponse);
            setCursor(&ili, 0, 170);
            writeText(&ili, str_to_lcd, 2);
            //sprintf(str_to_lcd,"1 Image/Sec: \n%d uWatt @ 1.2V   \n%d uWatt @ 1.0V   %c", (int)((float)(1/(50000000.f/ClusterCall.cycles)) * 28000.f),(int)((float)(1/(50000000.f/ClusterCall.cycles)) * 16800.f),'\0');
            //sprintf(out_perf_string,"%d  \n%d  %c", (int)((float)(1/(50000000.f/cycles)) * 28000.f),(int)((float)(1/(50000000.f/cycles)) * 16800.f),'\0');
        }
        #endif  /* USE_DISPLAY */
    }
    DEBUG_PRINTF("Test face detection done.\n");
    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS FaceDetection Test ***\n\n");
    return pmsis_kickoff((void *) test_facedetection);
}
