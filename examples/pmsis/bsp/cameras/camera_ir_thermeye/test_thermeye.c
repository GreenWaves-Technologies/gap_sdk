/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"

//#define ASYNC

/* App includes. */
#include "gaplib/ImgIO.h"

#define DEBUG_PRINTF(...) ((void) 0)

#define IMG_WIDTH  ( 80 )
#define IMG_HEIGHT ( 80 )
#define IMG_SIZE   ( IMG_HEIGHT * IMG_WIDTH )

static struct pi_device cam;

static uint16_t calib_buffer[IMG_SIZE];
static uint16_t img_buffer[IMG_SIZE];
static int16_t ref_buffer[IMG_SIZE];
static int16_t dif_buff[IMG_SIZE];
static uint8_t scaled_buff[IMG_SIZE];

static int32_t open_camera_thermeye(struct pi_device *device)
{
    struct pi_thermeye_conf cam_conf;
    pi_thermeye_conf_init(&cam_conf);
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}

void test_therm_eye()
{
    printf("Entering main controller\n");

    if (open_camera_thermeye(&cam))
    {
        printf("Thermal Eye camera open failed !\n");
        pmsis_exit(-1);
    }

    pi_task_t cb = {0};
    pi_task_block(&cb);

    printf("Cover sensor until end of calibration(led will switch off).\n");
    /* Calibration step. */
    pi_time_wait_us(3 * 1000 * 1000);
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);

    //printf("Buffer %p, size %d\n", calib_buffer, IMG_SIZE * sizeof(uint16_t));
    #if defined(ASYNC)
    pi_task_block(&cb);
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    pi_camera_capture_async(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t), &cb);
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_task_wait_on(&cb);
    #else
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_camera_capture(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t));
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    #endif  /* ASYNC */

    uint32_t frame_avg = 0;
    for (uint32_t i = 0; i < IMG_SIZE; i++)
    {
        frame_avg += calib_buffer[i];
    }
    printf("\nFrame avg %ld nb_pixels %ld\n", frame_avg, (uint32_t) IMG_SIZE);
    frame_avg = frame_avg / (uint32_t) IMG_SIZE;
    for (uint32_t i = 0; i < IMG_SIZE; i++)
    {
        ref_buffer[i] = calib_buffer[i] - frame_avg;
    }

    printf("Calibration done !\n");
    /* Wait some time after calibration. */
    pi_time_wait_us(2 * 1000 * 1000);

    uint32_t save_index = 0;
    uint32_t loop = 1;
    while (loop)
    {
        printf("\nCapture image %ld\n", loop);

        /* Capture image. */
        #if defined(ASYNC)
        pi_task_block(&cb);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_camera_capture_async(&cam, img_buffer, IMG_SIZE * sizeof(uint16_t), &cb);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&cb);
        #else
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, img_buffer, IMG_SIZE * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #endif  /* ASYNC */

        /* Process image. */
        int32_t min_diff = (1 << 14);
        int32_t max_diff = -(1 << 14);

        for (uint32_t i = 0; i < IMG_SIZE; i++)
        {
            dif_buff[i] = img_buffer[i] - ref_buffer[i];

            if (dif_buff[i] < 0 )
            {
                DEBUG_PRINTF("Negative value !\n");
                dif_buff[i] = 0;
            }
            if (dif_buff[i] > (1 << 14))
            {
                DEBUG_PRINTF("Saturation !\n");
                dif_buff[i] = (1 << 14);
            }

            if (dif_buff[i] > max_diff)
            {
                max_diff = dif_buff[i];
            }
            else if (dif_buff[i] < min_diff)
            {
                min_diff = dif_buff[i];
            }
        }
        uint32_t min_max_swing = max_diff - min_diff;
        for (uint32_t i = 0; i < IMG_SIZE; i++)
        {
            scaled_buff[i] = ((dif_buff[i] - min_diff) * 255) / min_max_swing;
        }

        char string_buffer[50];
        sprintf(string_buffer, "../../../img_%ld.ppm", save_index);
        WriteImageToFile(string_buffer, IMG_WIDTH, IMG_HEIGHT, 1, scaled_buff,GRAY_SCALE_IO);
        save_index++;
 
        loop++;
        pi_time_wait_us(1000);
    }

 
    pi_camera_close(&cam);

    printf("Test exit ...\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Therm Eye ***\n\n");
    return pmsis_kickoff((void *) test_therm_eye);
}
