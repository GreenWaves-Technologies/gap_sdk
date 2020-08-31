/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

// This example shows how to strean the capture of a camera image using
// small buffers.
// This is using aynchronous transfers to make sure a second buffer is always ready
// when the current is finished, to not lose any data.

#include "pmsis.h"
#include "ov7670_config.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/camera/ov7670.h"
#include "gaplib/ImgIO.h"

#include "bsp/display.h"
#include "bsp/display/ili9341.h"

#define WIDTH    320
#define HEIGHT   240
#define PIXEL_SIZE  2

#define BUFF_SIZE (WIDTH*HEIGHT*PIXEL_SIZE)

#define ITER_SIZE  (0x1FFFF)

PI_L2 unsigned char *buff;

static struct pi_device camera;
static struct pi_device lcd;
static pi_task_t ctrl_tasks[2];
static int remaining_size;
static volatile int done;
static int nb_transfers;
static unsigned char *current_buff;
static int current_task;
static pi_buffer_t buffer;

static void handle_transfer_end(void *arg);


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
        pi_camera_capture_async(&camera, current_buff, iter_size, pi_task_callback(task, handle_transfer_end, NULL));

        remaining_size -= iter_size;
        nb_transfers++;
        current_buff += iter_size;
        current_task ^= 1;
    }

}


static void handle_transfer_end(void *arg)
{
    nb_transfers--;

    enqueue_transfer();

    if (nb_transfers == 0)
        done = 1;
}

static int32_t open_display(struct pi_device *device)
{
    struct pi_ili9341_conf ili_conf;
    pi_ili9341_conf_init(&ili_conf);
    pi_open_from_conf(device, &ili_conf);
    if (pi_display_open(device))
    {
        printf("Failed to open display\n");
        return -1;
    }
    pi_display_ioctl(device, PI_ILI_IOCTL_ORIENTATION, (void *) PI_ILI_ORIENTATION_270);
    return 0;
}

typedef struct ov7670_reg{
    uint8_t addr;
    uint8_t value;
} ov7670_reg_t;

static ov7670_reg_t __ov7670_reg_QVGA_RGB[] =
{
    {REG_CLKRC, 0x01},
    {REG_COM7,  COM7_RGB|COM7_FMT_QVGA},
    {REG_COM3,  0x04},
    {REG_COM14, 0x19},
    {REG_COM15, 0xD0},
    {PLL_x4,    0x3A},
    {0x71,      0x35},
    {0x72,      0x11},
    {0x73,      0xf1},
    {0xA2,      0x02},
};

uint8_t reg_value=0;
static void set_camera_qvga_rgb_mode (struct pi_device *device)
{

    for(int i=0; i<(sizeof(__ov7670_reg_QVGA_RGB)/sizeof(ov7670_reg_t)); i++)
        pi_camera_reg_set(device, __ov7670_reg_QVGA_RGB[i].addr, &(__ov7670_reg_QVGA_RGB[i].value));

#if DEBUG
    for(int i=0; i<(sizeof(__ov7670_reg_QVGA_RGB)/sizeof(ov7670_reg_t)); i++)
    {
        pi_camera_reg_get(device, __ov7670_reg_QVGA_RGB[i].addr, &reg_value);
        printf("addr %X = %X\n", __ov7670_reg_QVGA_RGB[i].addr, reg_value);
    }
    pi_camera_reg_get(device, REG_COM2, &reg_value);
    printf("addr %X = %X\n", REG_COM2, reg_value);
#endif
}

static int open_camera(struct pi_device *device)
{
    printf("Opening OV7670 camera\n");
    struct pi_ov7670_conf cam_conf;
    pi_ov7670_conf_init(&cam_conf);

    cam_conf.format = PI_CAMERA_QVGA;

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
        return -1;

    set_camera_qvga_rgb_mode(device);
    return 0;
}

static void dump_cpi_reg()
{
    printf("addr glob: %X\n", (*(volatile unsigned int *)(long)(0x1A102480)));
    printf("size glob: %X\n", (*(volatile unsigned int *)(long)(0x1A102484)));
    printf("cfg glob: %X\n", (*(volatile unsigned int *)(long)(0x1A102488)));

    printf("reg glob: %X\n", (*(volatile unsigned int *)(long)(0x1A1024A0)));
    printf("ll glob: %X\n", (*(volatile unsigned int *)(long)(0x1A1024A4)));
    printf("ur glob: %X\n", (*(volatile unsigned int *)(long)(0x1A1024A8)));
    printf("size glob: %X\n", (*(volatile unsigned int *)(long)(0x1A1024AC)));
    printf("filter glob: %X\n", (*(volatile unsigned int *)(long)(0x1A1024B0)));
}

static void dump_pixel(unsigned char *buff, unsigned int size)
{
    for(int i=0; i<size; i++)
    {
        if (i && i%320==0)
            printf("\n");

        printf("0x%X ", buff[i]);
    }
    printf("\n");
}

static int test_entry()
{
    printf("Entering main controller\n");

    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        goto error;
    }

    if (open_display(&lcd))
    {
        printf("Failed to open display\n");
        pmsis_exit(-3);
    }

    buff = pmsis_l2_malloc(BUFF_SIZE);
    if (buff == NULL) goto error;

    for (int i=0; i<BUFF_SIZE; i++)
        buff[i] = 0x4F;

    remaining_size = BUFF_SIZE;
    nb_transfers = 0;
    current_buff = buff;
    current_task = 0;
    done = 0;

    buffer.data = buff;
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, buff);
    pi_buffer_set_format(&buffer, WIDTH, HEIGHT, 1, PI_BUFFER_FORMAT_RGB565);

    char name[25];
    int idx = 0;
    while (1)
    {
        enqueue_transfer();
        pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
        while(!done)
        {
            pi_yield();
        }
        pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);

#if defined(DISPLAY)
        pi_display_write(&lcd, &buffer, 0, 0, WIDTH, HEIGHT);
#else
        sprintf(name, "../../../output%d.raw", idx);
        WriteImageToFile(name, WIDTH, HEIGHT, PIXEL_SIZE, buff, RGB565_IO);
        idx++;
#endif  /* 0 */
        done = 0;
        remaining_size = BUFF_SIZE;
        nb_transfers = 0;
        current_buff = buff;
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
