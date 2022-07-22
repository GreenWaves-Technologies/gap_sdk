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
#include "bsp/camera/ov5640.h"
#include "ov5640_config.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

#include "gaplib/ImgIO.h"

#define WIDTH    320
#define HEIGHT   240
#define PIXEL_SIZE  2

#define BUFF_SIZE (WIDTH*HEIGHT*PIXEL_SIZE)
#define ITER_SIZE  (320*2*16)

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
    current_buff = (unsigned char)arg;

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
    struct pi_ov5640_conf cam_conf;
    pi_ov5640_conf_init(&cam_conf);

    cam_conf.format = PI_CAMERA_QVGA;

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
        return -1;

    return 0;
}

static int OV5640_get_sysclk(struct pi_device *device)
{
    // calculate sysclk
    uint8_t reg_value=0x4F;
    int temp2;
    int Multiplier, PreDiv, VCO, SysDiv, Pll_rdiv, Bit_div2x, sclk_rdiv, sysclk;
    int XVCLK = 8000000; //XCLK is 8MHz
    int sclk_rdiv_map[] = {
        1, 2, 4, 8};

    pi_camera_reg_get(device, 0x3034, &reg_value);
    temp2 = reg_value & 0x0f;               //0xA
    if (temp2 == 8 || temp2 == 10) {
        Bit_div2x = temp2 / 2;              //5
    }

    pi_camera_reg_get(device, 0x3035, &reg_value);
    SysDiv = reg_value>>4;                  //0x41>>4 = 0x4
    if(SysDiv == 0) {
        SysDiv = 16;
    }

    pi_camera_reg_get(device, 0x3036, &reg_value);
    Multiplier = reg_value;                 //0x90 = 144

    pi_camera_reg_get(device, 0x3037, &reg_value);
    PreDiv = reg_value & 0x0f;              //0x13&0xf = 3
    Pll_rdiv = ((reg_value >> 4) & 0x01) + 1; // 2

    pi_camera_reg_get(device, 0x3108, &reg_value);
    temp2 = reg_value & 0x03;               //1

    sclk_rdiv = sclk_rdiv_map[temp2];       //2
    VCO = XVCLK * Multiplier / PreDiv;      // 8MHz * 144 /3 = 384MHz
    sysclk = VCO / SysDiv / Pll_rdiv * 2 / Bit_div2x / sclk_rdiv;       // 384MHz/4/2*2/5/2 = 9.6MHz
    //printf("VCO %x SysDiv %x Pll_rdiv %x Bit_div2x %x sclk_rdiv %x Multiplier %x PreDiv %x\n", VCO , SysDiv , Pll_rdiv , Bit_div2x , sclk_rdiv, Multiplier, PreDiv);
    return sysclk;
}

static ov5640_reg_t __ov5640_test_pattern = {0x503D, 0x82};
static void ov5640_test_pattern(struct pi_device *device)
{
    uint8_t reg_value=__ov5640_test_pattern.value ;
    pi_camera_reg_set(device, __ov5640_test_pattern.addr, &(reg_value));
}

static int test_entry()
{
    printf("Entering main controller\n");
    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES);
    uint32_t cycles, tim_cycles;
    uint32_t start = pi_perf_read(PI_PERF_CYCLES);

    pi_perf_start();

    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        goto error;
    }

#ifdef DEBUG
    ov5640_test_pattern(&camera);
#endif
    printf("cam pclk: %d Hz\n", OV5640_get_sysclk(&camera));

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

    // prepare a full buffer for image IO
    imgIO_buff = pmsis_l2_malloc(BUFF_SIZE);

    if (buff[0] == NULL || buff[1] == NULL) goto error;

    remaining_size = BUFF_SIZE;
    nb_transfers = 0;
    current_buff = 0;
    current_task = 0;
    saved_size = 0;
    done = 0;

    char name[25];
    int idx = 0;


    unsigned int cfg_glob = (*(volatile unsigned int *)(long)(0x1A1024A0));
    cfg_glob |= ((0x3<<1)|0x1); // enable frame drop, and drop 1 image
    (*(volatile unsigned int *)(long)(0x1A1024A0)) = cfg_glob;

    //while (1)
    {
        enqueue_transfer();
        pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
        while(!done)
        {
            pi_yield();
        }
        pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);

        pi_perf_stop();
        cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
        tim_cycles = pi_perf_read(PI_PERF_CYCLES);
        printf("Perf : %d cycles Timer : %d cycles\n", cycles, tim_cycles);

        pi_ram_read(&ram, l3_buff, imgIO_buff, (uint32_t) (320*240*2));
        sprintf(name, "../../../output_%d.ppm", tim_cycles);
        WriteImageToFile(name, 320, 240, PIXEL_SIZE, imgIO_buff, RGB565_IO);
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
