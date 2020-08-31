/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

// This example shows how to strean the capture of a camera image using
// small buffers.
// This is using aynchronous transfers to make sure a second buffer is always ready
// when the current is finished, to not lose any data.

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/gc0308.h"

#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

#include "gaplib/ImgIO.h"
#include "bsp/display/ili9341.h"



struct pi_device ili;
struct pi_device device;
static pi_buffer_t buffer;

#define WIDTH    320
#define HEIGHT   240
#ifdef GRAYSCALE
#define PIXEL_SIZE  1
#else
#define PIXEL_SIZE  2
#endif

#define BUFF_SIZE (WIDTH*HEIGHT*PIXEL_SIZE)
#define ITER_SIZE  (0x2000)

PI_L2 unsigned char *buff[2];
PI_L2 unsigned char *imgIO_buff;

static struct pi_device ram;
static uint32_t l3_buff;
static struct pi_device camera;
static pi_task_t ctrl_tasks[2];

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
//            printf("%d\n", nb_transfers);
            pi_camera_capture_async(&camera, buff[current_task], iter_size, pi_task_callback(task, handle_transfer_end, (void *) current_task));

            current_size[current_task] = iter_size;
            remaining_size -= iter_size;
            nb_transfers++;
            current_task ^= 1;     
    }
}

static void handle_transfer_end(void *arg)
{
    nb_transfers--;
    current_buff = (unsigned char) arg;

    enqueue_transfer();

    pi_task_t cb_tx;
    pi_ram_write_async(&ram, (l3_buff+saved_size), buff[current_buff], (uint32_t) current_size[current_buff], pi_task_callback(&cb_tx, handle_ram_end, NULL));

    saved_size += current_size[current_buff];
}


static void handle_ram_end(void *arg)
{
    if (nb_transfers == 0 && saved_size == BUFF_SIZE)
        done = 1;
}

static int open_camera(struct pi_device *device)
{
    printf("Opening GC0308 camera\n");
    struct pi_gc0308_conf cam_conf;
    pi_gc0308_conf_init(&cam_conf);

    cam_conf.format = PI_CAMERA_QVGA;
    #ifdef GRAYSCALE 
    cam_conf.color_mode = PI_CAMERA_GRAY8;
    #else
    cam_conf.color_mode = PI_CAMERA_RGB565;
    #endif

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
        return -1;

    return 0;
}

typedef struct gc0308_reg{
    uint8_t addr;
    uint8_t value;
} gc0308_reg_t;

static gc0308_reg_t __gc0308_reg_init[] =
{

};

uint8_t reg_value=0;
static void test_camera_i2c (struct pi_device *device)
{

    for(int i=0; i<(sizeof(__gc0308_reg_init)/sizeof(gc0308_reg_t)); i++)
        pi_camera_reg_set(device, __gc0308_reg_init[i].addr, &(__gc0308_reg_init[i].value));

    for(int i=0; i<(sizeof(__gc0308_reg_init)/sizeof(gc0308_reg_t)); i++)
    {
        pi_camera_reg_get(device, __gc0308_reg_init[i].addr, &reg_value);
        printf("addr %X = %X\n", __gc0308_reg_init[i].addr, reg_value);
    }
}


static int open_display(struct pi_device *device)
{
  struct pi_ili9341_conf ili_conf;

  pi_ili9341_conf_init(&ili_conf);

  pi_open_from_conf(device, &ili_conf);

  if (pi_display_open(device))
    return -1;

  if (pi_display_ioctl(device, PI_ILI_IOCTL_ORIENTATION, (void *)PI_ILI_ORIENTATION_90))
    return -1;

  return 0;
}


static int test_entry()
{

    printf("Entering main controller\n");
    // prepare a full buffer for image IO
    imgIO_buff = pmsis_l2_malloc(BUFF_SIZE);
    pi_freq_set(PI_FREQ_DOMAIN_FC,250000000);

    
    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        goto error;
    }
    //pi_camera_set_crop(&camera,160,120,320,240);

    
    #ifdef OUT_TO_DISPLAY
    if (open_display(&ili))
    {
        printf("Failed to open display\n");
        pmsis_exit(-1);
    }

    writeFillRect(&ili, 0, 0, 320, 240, 0xFFFF);
    writeText(&ili, "    GreenWaves", 3);
    writeText(&ili, "\n   Technologies", 3);
    buffer.data = imgIO_buff;
    buffer.stride = 0;

    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgIO_buff);
    pi_buffer_set_stride(&buffer, 0);
    #ifdef GRAYSCALE
    pi_buffer_set_format(&buffer, 320, 240, 2, PI_BUFFER_FORMAT_GRAY);
    #else    
    pi_buffer_set_format(&buffer, 320, 240, 2, PI_BUFFER_FORMAT_RGB565);
    #endif

    #endif

    #ifdef STORE_IMG_TO_L3

    /* Init & open ram. */
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    if (pi_ram_alloc(&ram, &l3_buff, (uint32_t) BUFF_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }    

    // prepare 2 buffer for double buffering
    buff[0] = pmsis_l2_malloc(ITER_SIZE);
    buff[1] = pmsis_l2_malloc(ITER_SIZE);
    if (buff[0] == NULL || buff[1] == NULL) goto error;

    
    remaining_size = BUFF_SIZE;
    nb_transfers = 0;
    current_buff = 0;
    current_task = 0;
    saved_size = 0;
    done = 0;
    #endif

    char name[25];
    int idx = 0;

    while (1)
    {
        #ifdef STORE_IMG_TO_L3 //This needs to be fixed

            enqueue_transfer();
            pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
            while(!done) pi_yield();
            pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);

        #else

            pi_task_t task_1;
            pi_task_t task_2;
            pi_camera_capture_async(&camera, imgIO_buff, BUFF_SIZE/2,pi_task_block(&task_1));
            pi_camera_capture_async(&camera, imgIO_buff+(BUFF_SIZE/2), BUFF_SIZE/2,pi_task_block(&task_2));
            pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
            pi_task_wait_on(&task_1);
            pi_task_wait_on(&task_2);
            pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);

        #endif

        #ifndef OUT_TO_DISPLAY
            printf("Saving Image to file...\n");
            //pi_ram_read(&ram, l3_buff, imgIO_buff, (uint32_t) BUFF_SIZE);
            sprintf(name, "../../../output_%d.ppm", idx);

            #ifdef GRAYSCALE
            WriteImageToFile(name, WIDTH, HEIGHT, PIXEL_SIZE, imgIO_buff, GRAY_SCALE_IO);
            #else
            WriteImageToFile(name, WIDTH, HEIGHT, PIXEL_SIZE, imgIO_buff, RGB565_IO);
            #endif
        #else
            pi_display_write(&ili, &buffer, 0,0, 320, 240);   
        #endif

        idx++;

        done = 0;
        remaining_size = BUFF_SIZE;
        saved_size = 0;
        nb_transfers = 0;
        current_buff = 0;
        current_task = 0;
    }

    pi_camera_close(&camera);

    printf("Test success\n");

    return 0;

error:
    return -1;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
