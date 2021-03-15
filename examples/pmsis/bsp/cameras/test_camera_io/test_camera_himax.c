#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"

/* Demo includes. */
#include "gaplib/ImgIO.h"

#ifdef SLICE_MODE
#define X                   60 
#define Y                   60 
#define CAMERA_WIDTH        260
#define CAMERA_HEIGHT       180
#else
#define CAMERA_WIDTH        324
#ifdef QVGA_IMG
#define CAMERA_HEIGHT       224
#else // max resolution of Himax camera
#define CAMERA_HEIGHT       324
#endif /* QVGA */
#endif

static pi_task_t task;
static uint8_t *imgBuff0;
static struct pi_device cam;

static char imgName[50];
static uint32_t idx = 0;

static void cam_handler(void *arg);
static void lcd_handler(void *arg);

static void dump_reg()
{
    printf("reg CFG_GLOB: %x \n", *(volatile unsigned int *)(long)(0x1A1024A0));
    printf("reg CFG_LL: %x \n", *(volatile unsigned int *)(long)(0x1A1024A4));
    printf("reg CFG_UR: %x \n", *(volatile unsigned int *)(long)(0x1A1024A8));
    printf("reg CFG_SIZE: %x \n", *(volatile unsigned int *)(long)(0x1A1024AC));
    printf("reg CFG_FILTER: %x \n", *(volatile unsigned int *)(long)(0x1A1024B0));
}

static void cam_handler(void *arg)
{
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

    sprintf(imgName, "../../../img_OUT_%ld.ppm", idx);
    WriteImageToFile(imgName, CAMERA_WIDTH, CAMERA_HEIGHT, sizeof(uint8_t), imgBuff0, GRAY_SCALE_IO);
    idx++;
}

static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);

#ifdef SLICE_MODE
    cam_conf.roi.slice_en = 1;
    cam_conf.roi.x = X;
    cam_conf.roi.y = Y;
    cam_conf.roi.w = CAMERA_WIDTH;
    cam_conf.roi.h = CAMERA_HEIGHT;
#endif

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }

    /* Let the camera AEG work for 100ms */
    pi_camera_control(&cam, PI_CAMERA_CMD_AEG_INIT, 0);

    return 0;
}

void test_camera_with_lcd(void)
{
    printf("Entering main controller...\n");
    int32_t errors = 0;

    imgBuff0 = (uint8_t *) pmsis_l2_malloc((CAMERA_WIDTH * CAMERA_HEIGHT) * sizeof(uint8_t));
    if (imgBuff0 == NULL)
    {
        printf("Failed to allocate Memory for Image, asking for: %d * %d\n", CAMERA_WIDTH, CAMERA_HEIGHT);
        pmsis_exit(-1);
    }

    errors = open_camera_himax(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }

#ifdef DEBUG_REG 
    dump_reg();
#endif

    while (1)
    {
        #if defined(ASYNC)
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_task_callback(&task, cam_handler, NULL);
        pi_camera_capture_async(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT, &task);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
        #else
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

        sprintf(imgName, "../../../img_OUT_%ld.ppm", idx);
        printf("Dumping image %s\n", imgName);
        WriteImageToFile(imgName, CAMERA_WIDTH, CAMERA_HEIGHT, sizeof(uint8_t), imgBuff0, GRAY_SCALE_IO);
        idx++;
        #endif  /* ASYNC */
    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with Himax ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}
