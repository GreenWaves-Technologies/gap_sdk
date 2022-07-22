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
#define ITER_SIZE  (0x12C00)

/* statics */

static uint8_t* img_buffers[2];
static volatile int current_idx = 0;
static volatile int next_idx = 0;

static struct pi_device cam;

static struct pi_device ram;
static uint32_t l3_buff;
static pi_task_t ctrl_tasks[2];
static pi_task_t ram_tasks[2];

static int remaining_size;
static int saved_size;
static volatile int done;
static int nb_transfers;
static unsigned char current_buff;
static int current_size[2];
static int current_task;

static void handle_transfer_end(void *arg);
static void handle_ram_end(void *arg);

// This is called to enqueue new transfers
static void enqueue_transfer()
{
    // We can enqueue new transfers if there are still a part of the image to
    // capture and less than 2 transfers are pending (the dma supports up to 2 transfers
    // at the same time)
    while (remaining_size > 0 && nb_transfers < 2)
    {
            int iter_size = ITER_SIZE;
            if (remaining_size < iter_size)
                iter_size = remaining_size;

            pi_task_t *task = &ctrl_tasks[current_task];
            // Enqueue a transfer. The callback will be called once the transfer is finished
            // so that  a new one is enqueued while another one is already running
            pi_camera_capture_async(&cam, img_buffers[current_task], iter_size, pi_task_callback(task, handle_transfer_end, (void *) current_task));

            current_size[current_task] = iter_size;
            remaining_size -= iter_size;
            nb_transfers++;
            current_task ^= 1;     
    }
}

static void handle_transfer_end(void * arg)
{
    nb_transfers--;
    unsigned char last = arg;
    int size = current_size[last];

    enqueue_transfer();

    pi_task_t *cb_tx = &ram_tasks[last];
    pi_ram_write_async(&ram, (l3_buff+saved_size), img_buffers[last], (uint32_t) size, pi_task_callback(cb_tx, handle_ram_end, NULL));

    saved_size += size;
}

static void handle_ram_end(void *arg)
{
    if (nb_transfers == 0 && saved_size == (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE))
        done = 1;
}



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

static void process_image(int idx)
{
    //WriteImageToFile("../../../capture.ppm", CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_PIXEL_SIZE, image_buffer, GRAY_SCALE_IO);

    char imgName[50];
    sprintf(imgName, "../../../img_OUT_%ld.ppm", idx);
    WriteImageToFileL3(&ram, imgName, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_PIXEL_SIZE, l3_buff, GRAY_SCALE_IO);
}

static int32_t open_camera(struct pi_device *device)
{
    return open_camera_hm0360(device);
}

static void test_camera_double_buffer(void)
{
    printf("Launching test ...\n");
    int32_t errors = 0;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

    struct pi_spiram_conf conf;
    pi_spiram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    if (pi_ram_alloc(&ram, &l3_buff, (uint32_t) (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE)))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }

    /* allocate memory for images */

    for(int i = 0; i<2; i++)
    {
        img_buffers[i] = (uint8_t *) pmsis_l2_malloc(ITER_SIZE);
        if (img_buffers[i] == NULL)
        {
            printf("Failed to allocate Memory for Image %d \n", i);
            pmsis_exit(-1);
        }
    }

    remaining_size = (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE);
    nb_transfers = 0;
    current_buff = 0;
    current_task = 0;
    saved_size = 0;
    done = 0;

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
        enqueue_transfer();

        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        while(!done) pi_yield();
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        printf("capture done\n");

        process_image(idx);

        remaining_size = (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE);
        nb_transfers = 0;
        current_buff = 0;
        current_task = 0;
        saved_size = 0;
        done = 0;
    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera software double buffering ***\n\n");
    return pmsis_kickoff((void *) test_camera_double_buffer);
}
