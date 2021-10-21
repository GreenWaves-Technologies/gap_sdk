/*
 * Copyright 2020 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pmsis.h"
#include "bsp/fs.h"


#define NUM_OFFSET_IMG 2
#define TIME_DELAY_OFFSET_IMG 2

#define NUM_TEST_IMG 5

#define IMG_WIDTH  ( 80 )
#define IMG_HEIGHT ( 80 )
#define IMG_SIZE   ( IMG_HEIGHT * IMG_WIDTH )


static struct pi_device cam;

static uint16_t calib_buffer[IMG_SIZE];
static uint32_t img_buffer[IMG_SIZE];
static int16_t ref_buffer[IMG_SIZE];
static int16_t dif_buff[IMG_SIZE];
static uint8_t scaled_buff[IMG_SIZE];

#define GPIO_USER_LED 0

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
    
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
    pi_freq_set(PI_FREQ_DOMAIN_FC,250000000);

    if (open_camera_thermeye(&cam))
    {
        printf("Thermal Eye camera open failed !\n");
        pmsis_exit(-1);
    }

    pi_task_t cb = {0};
    pi_task_block(&cb);

    printf("Cover sensor until end of offset image collection(led will switch off).\n");
    /* Calibration step. */
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
    pi_time_wait_us(3 * 1000 * 1000);
    //printf("Buffer %p, size %d\n", calib_buffer, IMG_SIZE * sizeof(uint16_t));

    for(int i=0;i>IMG_SIZE;i++) img_buffer[i]=0;

    for(int j=0;j<NUM_OFFSET_IMG;j++){

        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

        for(int i=0;i>IMG_SIZE;i++){
            img_buffer[i]+=calib_buffer[i];
        }
        printf("Collected image %d/%d to create offset image\n", j+1, NUM_OFFSET_IMG);
        pi_time_wait_us(TIME_DELAY_OFFSET_IMG * 1000 * 1000);
    }
    
    for(int i=0;i>IMG_SIZE;i++){
        calib_buffer[i] = img_buffer[i]/NUM_OFFSET_IMG;
    }

    pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
    printf("Offset Collected, now collecting raw dataset images!\n");
    
    
    char string_buffer[50];
    struct pi_fs_conf conf;
    pi_fs_conf_init(&conf);
    struct pi_device fs;
    conf.type = PI_FS_HOST;
    pi_open_from_conf(&fs, &conf);
    if (pi_fs_mount(&fs)) return ;
    
    {
        sprintf(string_buffer, "../../../offset_img/Calibration.bin");
        
        void *File = pi_fs_open(&fs, string_buffer, PI_FS_FLAGS_WRITE);    
        #define CHUNK_SIZE 1024
        unsigned char * OutBuffer = (unsigned char * ) calib_buffer;
        int datasize = IMG_SIZE * sizeof(uint16_t);
        int steps = datasize / CHUNK_SIZE;


        for (int i=0; i<steps; i++)
        {
            pi_fs_write(File,OutBuffer +(CHUNK_SIZE*i), CHUNK_SIZE);
        }
        if (((datasize) % CHUNK_SIZE) != 0)
        {
            pi_fs_write(File,OutBuffer+(CHUNK_SIZE*steps) , ((datasize) % CHUNK_SIZE)*sizeof(unsigned char));
        }
        pi_fs_close(File);
    }
    
    int save_index=0;
    int num_img=NUM_TEST_IMG;
    while(num_img--){

        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

        sprintf(string_buffer, "../../../dump_out_imgs/img_%04ld.bin", save_index);
        printf("writing image %04ld to disk\n",save_index);        
        void *File = pi_fs_open(&fs, string_buffer, PI_FS_FLAGS_WRITE);    
        #define CHUNK_SIZE 1024
        unsigned char * OutBuffer = (unsigned char * ) calib_buffer;
        int datasize = IMG_SIZE * sizeof(uint16_t);
        int steps = datasize / CHUNK_SIZE;


        for (int i=0; i<steps; i++)
        {
            pi_fs_write(File,OutBuffer +(CHUNK_SIZE*i), CHUNK_SIZE);
        }
        if (((datasize) % CHUNK_SIZE) != 0)
        {
            pi_fs_write(File,OutBuffer+(CHUNK_SIZE*steps) , ((datasize) % CHUNK_SIZE)*sizeof(unsigned char));
        }
        pi_fs_close(File);
        save_index++;
    }

    
    pi_fs_unmount(&fs);
    printf("Exit ...\n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Therm Eye Calibration Software ***\n\n");
    return pmsis_kickoff((void *) test_therm_eye);
}
