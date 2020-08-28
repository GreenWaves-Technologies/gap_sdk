#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"

/* constants */

#define CAMERA_WIDTH  ( 324 )
#define CAMERA_HEIGHT ( 244 )

/* statics */

static pi_task_t task;

static uint8_t* img_buffers[2];
static volatile int current_idx = 0;
static volatile int next_idx = 0;

static struct pi_device cam;

/* static functions */

static void cam_handler(void *arg)
{
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
}

#if defined(HIMAX)
static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}
#else
#error "supports only HIMAX camera for now"
#endif  /* HIMAX */

static void process_image(uint8_t* image_buffer)
{
    uint32_t sum = 0;
    /* example processing, not really interesting */
    for(int i = 0; i < CAMERA_HEIGHT * CAMERA_WIDTH; i++)
    {
        sum += image_buffer[i];
    }
    printf("Pixel sum: %d\n", sum);
}

static int32_t open_camera(struct pi_device *device)
{
    return open_camera_himax(device);
}

static void test_camera_double_buffer(void)
{
    printf("Launching test ...\n");
    int32_t errors = 0;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

    /* allocate memory for images */

    for(int i = 0; i<2; i++)
    {
        img_buffers[i] = (uint8_t *) pmsis_l2_malloc((CAMERA_WIDTH * CAMERA_HEIGHT) * sizeof(uint8_t));
        if (img_buffers[i] == NULL)
        {
            printf("Failed to allocate Memory for Image \n");
            pmsis_exit(-1);
        }
    }

    /* open camera */

    errors = open_camera(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }

    /* make sure that the camera is not sending data */
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

    /* we first need to capture an image before launching double buffer loop */
    pi_task_callback(&task, cam_handler, NULL);
    pi_camera_capture_async(&cam, img_buffers[next_idx], CAMERA_WIDTH * CAMERA_HEIGHT, &task);
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_task_wait_on(&task);

    /* capture images */
    printf("Main loop start\n");
    for(int i = 0; i < 100; i++)
    {
        current_idx = next_idx;
        next_idx ^= 1;

        pi_task_callback(&task, cam_handler, NULL);
        /* launch the capture of the next image */
        pi_camera_capture_async(&cam, img_buffers[next_idx], CAMERA_WIDTH * CAMERA_HEIGHT, &task);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);

        /* processing current image */
        process_image(img_buffers[current_idx]);

        pi_task_wait_on(&task);
    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera software double buffering ***\n\n");
    return pmsis_kickoff((void *) test_camera_double_buffer);
}
