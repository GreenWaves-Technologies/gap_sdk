/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/fs.h"

/* Demo includes. */
#include "setup.h"

#include "gaplib/ImgIO.h"


static pi_task_t task;
static uint8_t *imgBuff0;
static struct pi_device cam;

static void cam_handler(void *arg);

static void cam_handler(void *arg)
{
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
}

// pixart
static int32_t open_camera_pixart(struct pi_device *device)
{
    struct pi_pixart_conf cam_conf;
    pi_pixart_conf_init(&cam_conf);
    #if defined(QVGA)
    cam_conf.format = PI_CAMERA_QVGA;
    #endif  /* QVGA */
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}

static int32_t open_camera(struct pi_device *device)
{
    return open_camera_pixart(device);
    return -1;
}

void test_camera_with_lcd(void)
{
    printf("Entering main controller...\n");

    //pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

    imgBuff0 = (uint8_t *) pmsis_l2_malloc((320 * 240) * sizeof(uint8_t));
    if (imgBuff0 == NULL)
    {
        printf("Failed to allocate Memory for Image \n");
        pmsis_exit(-1);
    }

    if (open_camera(&cam))
    {
        printf("Failed to open camera\n");
        pmsis_exit(-2);
    }

    uint8_t index = 0;
    printf("Main loop start\n");
    while (1)
    {
        #if defined(ASYNC)
        printf("ASYNC...\n");
        printf("Camera stop.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_task_callback(&task, cam_handler, NULL);
        printf("Image capture.\n");
        pi_camera_capture_async(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT, &task);
        printf("Camera start.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
        #else
        printf("SYNC...\n");
        printf("Camera start.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        printf("Image captured.\n");
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT);
        printf("Camera stop.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #endif

        char filename[30];
        sprintf(filename, "../../../pgmfile%d.pgm", index);
        WriteImageToFile(filename, 320, 240, sizeof(uint8_t), imgBuff0, GRAY_SCALE_IO);

        index++;
    }

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with LCD Test ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}
