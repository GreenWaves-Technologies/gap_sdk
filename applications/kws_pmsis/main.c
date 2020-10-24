/*
 * Copyright (C) 2018 GreenWaves Technologies
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

/*
 * This example shows how to capture PDM samples asynchronously using callbacks
 */

#include "pmsis.h"
#include "wav_out.h"
#include <bsp/bsp.h>
#include "MFCC_Processing.h"


/* Autotiler includes. */
#include "Gap.h"
#include "kwsKernels.h"
#include "ImgIO.h"

#define USE_AUDIO
#define NB_ELEM 16000
#define BUFF_SIZE (NB_ELEM*2)
#define ITER   1

#define GPIO_BUTTON   PI_GPIO_A0_PAD_12_A3

// allocate ping pong buffer for sampling the 1 stereo channels
static uint16_t buff[2][BUFF_SIZE];

// buffer to store 1s of single channel mono audio signal
uint16_t dump_buff[BUFF_SIZE];

static struct pi_device i2s;
static struct pi_i2s_conf i2s_conf;
static uint16_t * audioBuff;

static struct pi_device gpio;



/*******************************************************************************
 * Frame Variables
 ******************************************************************************/
//GAP_L2_DATA v2s  W_Frame[N_FFT+4];
PI_FC_L1 v2s  *W_Frame;
//GAP_L2_DATA  short int Frame[FRAME];
PI_FC_L1  short int *Frame;
short int FEAT_LIST[N_FRAME * NUMCEP];
short int *pfeat_list;
PI_L2 short int *ResOut;
#define STACK_SIZE      1024
AT_HYPERFLASH_FS_EXT_ADDR_TYPE kws_L3_Flash = 0;

static void Runkws_clust()

{

    kwsCNN(pfeat_list, ResOut);
    int max=0x80000000;
    int indx=-1;
    for (int i=0; i<12;i++) {
        if (ResOut[i]>max) {max=ResOut[i]; indx=i;}
    }
    printf("found %d\n",indx);

}


void runkws(char trial) {

#ifdef DOMFCC
    short int *InSignal = (short int *) (audioBuff);

    // MFCC
    W_Frame = pi_fc_l1_malloc((N_FFT+4)*sizeof(v2s));
    Frame = pi_fc_l1_malloc(FRAME*sizeof(short int));


    if(!Frame || !W_Frame)
    {
        printf("Error in fc l1 malloc\n");
        pmsis_exit(-2);
    }

    MFCC_Processing(InSignal, W_Frame, Frame, FEAT_LIST);
    pfeat_list = (short int*) FEAT_LIST;

    pi_fc_l1_free(Frame,(N_FFT+4)*sizeof(v2s));
    pi_fc_l1_free(W_Frame,FRAME*sizeof(short int));

#else
    pfeat_list = (short int*) DataIn;
#endif

    // CNN
    ResOut = (short int *) pi_l2_malloc(12 * sizeof(short int));
    pfeat_list=(short int*) FEAT_LIST;
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 12*sizeof(short int));
        pmsis_exit(-3);
    }

    struct pi_device cluster_dev = {0};
    struct pi_cluster_conf cl_conf = {0};

    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;

    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }

    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (kwsCNN_Construct())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-5);
    }
    struct pi_cluster_task task = {0};
    task.entry = Runkws_clust;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    kwsCNN_Destruct();
    // Close the cluster
    pi_cluster_close(&cluster_dev);

    pi_l2_free(ResOut,12*sizeof(short));


}

// dump one mono channel chunk (NB_ELEM/2 16bits samples) from the i2s0 interface in dump_buff:
//
void init_gpio()
{
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&gpio, &gpio_conf);
    if (pi_gpio_open(&gpio))
    {
        printf("Error opening GPIO\n");
        pmsis_exit(-1);
    }

    pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;
    pi_gpio_pin_configure(&gpio, GPIO_BUTTON, cfg_flags);
    pi_gpio_pin_notif_configure(&gpio, GPIO_BUTTON, irq_type);

}

int init_I2S() {

    for(uint32_t i=0; i<NB_ELEM;i++)
    {
        buff[0][i] = 0;
        buff[1][i] = 0;
    }

    pi_bsp_init();

    // Get default I2S interface config
    //struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    // Configure it for PDM 16Khz
    // Also gives the 2 buffers for double-buffering the sampling
    // PDM samples will be demodulated automatically by the driver
    i2s_conf.pingpong_buffers[0] = buff[0];
    i2s_conf.pingpong_buffers[1] = buff[1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = 16000;
    i2s_conf.itf = 0;
    i2s_conf.channels = 2;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = 16;
    i2s_conf.pdm_shift = 4;

    pi_open_from_conf(&i2s, &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s))
        return -1;

    return 0;
}

int nb_test=0;

int capture_and_process() {


    unsigned int size;
    uint32_t value;
    char dump_file_name[25];
    short *audio_chunk;

    printf(">>> Please press the button to start\n");
    while(1)
    {
        pi_gpio_pin_read(&gpio, GPIO_BUTTON, &value);
        if(!value)
        {
            // when the button be pressed, the value will be 1

            // Capture samples
            if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
                return -1;


            pi_i2s_read(&i2s, (void **)&audioBuff, &size);

            printf("dump wave file ...\n");
            sprintf(dump_file_name, "../../../output%d.wav", nb_test);
            dump_wav(dump_file_name, i2s_conf.word_size, i2s_conf.frame_clk_freq,
                    i2s_conf.channels, (void *) (audioBuff), BUFF_SIZE);
            printf("dump wave done\n");

            runkws(0);

            printf(">>> Done: %d\n", nb_test++);
            printf(">>> Please press the button to start\n");
        }
    }
}

static int test_entry()
{
    printf("Entering main controller\n");
    init_gpio();
    init_I2S();

    capture_and_process();

    return 0;

}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
