/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
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

#include "pmsis.h"
#include "kiss_fft.h"



#ifndef NB_ITF
#define NB_ITF 1
#endif

#ifndef NB_CHANNELS
#define NB_CHANNELS 1
#endif

#define ERROR_RATE 20
#if WORD_SIZE == 32
#define ELEM_SIZE 4
#elif WORD_SIZE == 16
#define ELEM_SIZE 2
#else
#error Unsupported word size
#endif

#define NB_ELEM (NB_ELEM_PER_CHANNEL*NB_CHANNELS)

#define BUFF_SIZE (NB_ELEM*ELEM_SIZE)

#define CAPTURE_SIZE (BUFF_SIZE*NB_CAPTURE*NB_START_STOP)

static uint8_t buff[NB_ITF][2][BUFF_SIZE*NB_CHANNELS];
static uint8_t *ch_buff[NB_ITF][NB_CHANNELS];
static struct pi_device i2s[NB_ITF];
static int max[NB_ITF][NB_CHANNELS];
static int min[NB_ITF][NB_CHANNELS];
static pi_task_t task[NB_ITF];
static int nb_captured[NB_ITF];
static int nb_captured_elem[NB_ITF];
static int end;



static kiss_fft_cpx buff_complex[NB_ELEM];
static kiss_fft_cpx buff_complex_out[NB_ELEM];
static kiss_fft_cfg cfg;

static int get_sampling_freq(int itf)
{
  if (itf == 0)
    return SAMPLING_FREQ_0;
  else
    return SAMPLING_FREQ_1;
}


static int check_error(int bound, int value, float error_margin)
{
  float error = (float)(bound - value) / bound;
  if (error < 0)
    error = -error;

  printf("    Got bound error rate %f (bound: 0x%x, got: 0x%x)\n", error, bound, value);

  return error > error_margin;
}


static int get_signal_freq(int itf, int channel)
{
  if (itf == 0)
  {
//#if defined(MODE) && MODE == USE_2CH
//    if (channel == 0)
//      return SIGNAL_FREQ_0_0;
//    else if (channel == 2)
//      return SIGNAL_FREQ_0_1;
//    else if (channel == 1)
//      return SIGNAL_FREQ_1_0;
//    else
//      return SIGNAL_FREQ_1_1;
//#else
    if (channel == 0)
      return SIGNAL_FREQ_0_0;
    else if (channel == 1)
      return SIGNAL_FREQ_0_1;
//#endif
  }
  else
    if (channel == 0)
      return SIGNAL_FREQ_1_0;
    else
      return SIGNAL_FREQ_1_1;
}

static int check_buffer(uint8_t *buff, int sampling_freq, int signal_freq)
{
  for (int i=0; i<NB_ELEM; i++)
  {
    //printf("%x\n", (((int16_t *)buff)[i+16]));

#if WORD_SIZE <= 16
    buff_complex[i].r = (float)(((int16_t *)buff)[i]);
#else
    buff_complex[i].r = (float)(((int32_t *)buff)[i]);
#endif
    buff_complex[i].i = 0.0;
  }

  // Get signal frequencies
  kiss_fft(cfg, buff_complex, buff_complex_out);

  float max = 0;
  int index = -1;

  // Find the most important one
  for (int i=0; i<NB_ELEM; i++)
  {
    float val = buff_complex_out[i].r*buff_complex_out[i].r*1.0/sampling_freq + buff_complex_out[i].i*buff_complex_out[i].i*1.0/sampling_freq;
    if (index == -1 || val > max)
    {
      index = i;
      max = val;
    }
  }

  float freq = (int)((float)index / NB_ELEM * sampling_freq);
  float error = (freq - signal_freq) / signal_freq * 100;
  if (error < 0)
    error = -error;

  printf("    Got error rate %d %% (buffer: %p, expected: %d, got %d)\n", (int)error, buff, (int)signal_freq, (int)freq);

  return error > ERROR_RATE;
}


static void handle_read(void *arg);



static void handle_resume(void *arg)
{
  int itf_id = (int)arg;
  pi_i2s_ioctl(&i2s[itf_id], PI_I2S_IOCTL_START, NULL);
  pi_i2s_read_async(&i2s[itf_id], pi_task_callback(&task[itf_id], handle_read, (void *)itf_id));
}


static void handle_read(void *arg)
{

  int itf_id = (int)arg;

  nb_captured[itf_id]--;

  void *mem_block;
  int size;

  if (pi_i2s_read_status(&task[itf_id], &mem_block, &size))
    return;

  char *buffer = (char *)mem_block;

  for (int k=0; k<NB_CHANNELS; k++)
  {
    min[itf_id][k] = 0;
    max[itf_id][k] = 0;

    for (int j=0; j<NB_ELEM; j++)
    {
      int elem_value = 0;

      for (int l=0; l<ELEM_SIZE; l++)
      {
        int value = buffer[(j*NB_CHANNELS + k)*ELEM_SIZE+l];
        ch_buff[itf_id][k][nb_captured_elem[itf_id]*ELEM_SIZE + j*ELEM_SIZE+l] = value;

        elem_value |= value << (l*8);
      }

      if (WORD_SIZE == 16)
      {
        elem_value = (elem_value << 16) >> 16;
      }

      if (elem_value > max[itf_id][k])
        max[itf_id][k] = elem_value;
      if (elem_value < min[itf_id][k])
        min[itf_id][k] = elem_value;
    }

  }

  nb_captured_elem[itf_id] += NB_ELEM;


  if (nb_captured[itf_id] == 0)
  {
    end--;
  }
  else if (nb_captured[itf_id] % NB_CAPTURE == 0)
  {
    pi_i2s_ioctl(&i2s[itf_id], PI_I2S_IOCTL_STOP, NULL);
    pi_task_push_delayed_us(pi_task_callback(&task[itf_id], handle_resume, (void *)itf_id), 1000*(itf_id+1));
  }
  else
  {
    pi_i2s_read_async(&i2s[itf_id], pi_task_callback(&task[itf_id], handle_read, (void *)itf_id));
  }

}

static int test_entry()
{
  int errors = 0;

  printf("Entering test (pdm: %d, nb_itf: %d, nb_channels: %d, word_size: %d, nb_elem: %d, nb_capture: %d, nb_start_stop: %d)\n", PDM, NB_ITF, NB_CHANNELS, WORD_SIZE, NB_ELEM_PER_CHANNEL, NB_CAPTURE, NB_START_STOP);

  pi_bsp_init();

  pi_pad_set_function(PI_PAD_37_B14_I2S1_SDI, PI_PAD_37_B14_I2S1_SDI_FUNC0);
  pi_pad_set_function(PI_PAD_36_A15_I2S1_WS, PI_PAD_36_A15_I2S1_WS_FUNC0);
  pi_pad_set_function(PI_PAD_35_B13_I2S1_SCK, PI_PAD_35_B13_I2S1_SCK_FUNC0);
  pi_pad_set_function(PI_PAD_55_A24_I2S0_SCK, PI_PAD_55_A24_I2S0_SCK_FUNC0);
  pi_pad_set_function(PI_PAD_56_A26_I2S0_WS, PI_PAD_56_A26_I2S0_WS_FUNC0);
  pi_pad_set_function(PI_PAD_57_B23_I2S0_SDI, PI_PAD_57_B23_I2S0_SDI_FUNC0);


  for (int i=0; i<NB_ITF; i++)
  {
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.pingpong_buffers[0] = buff[i][0];
    i2s_conf.pingpong_buffers[1] = buff[i][1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = get_sampling_freq(i);
    i2s_conf.itf = i;
#if defined(PDM) && PDM == 1
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.pdm_decimation = 64;
#else
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;
#endif
    i2s_conf.word_size = WORD_SIZE;
    i2s_conf.channels = NB_CHANNELS;

    pi_open_from_conf(&i2s[i], &i2s_conf);

    if (pi_i2s_open(&i2s[i]))
      return -1;

    nb_captured[i] = NB_CAPTURE*NB_START_STOP;
    nb_captured_elem[i] = 0;

    for (int j=0; j<NB_CHANNELS; j++)
    {
      ch_buff[i][j] = pi_l2_malloc(CAPTURE_SIZE);
      if (ch_buff[i][j] == NULL)
        return -1;
    }
  }

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {
      min[i][k] = 0;
      max[i][k] = 0;
    }
  }

  for (int i=0; i<NB_ITF; i++)
  {
    if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
      return -1;
  }

  end = NB_ITF;

  for (int i=0; i<NB_ITF; i++)
  {
    pi_i2s_read_async(&i2s[i], pi_task_callback(&task[i], handle_read, (void *)i));
  }

  while(*(int *)&end)
  {
    pi_yield();
  }

  for (int i=0; i<NB_ITF; i++)
  {
    pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s[i]);
    for (int j=0; j<NB_CHANNELS; j++)
    {
      pi_l2_free(ch_buff[i][j], CAPTURE_SIZE);
    }
  }

  cfg = kiss_fft_alloc(NB_ELEM, 0, NULL, NULL);

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {
      printf("Checking itf %d channel %d\n", i, k);

      if (check_error((1ULL<<(WORD_SIZE-1)) - 1, max[i][k], 0.2))
        return -1;

      if (check_error(-(1LL<<(WORD_SIZE-1)), min[i][k], 0.2))
        return -1;

      for (int l=1; l<NB_CAPTURE*NB_START_STOP; l++)
      {
        char *buff = &ch_buff[i][k][l*BUFF_SIZE];

#if 0
        printf("Raw buffer size\n");
        for (int j=0; j<BUFF_SIZE/ELEM_SIZE/NB_CHANNELS; j++)
        {
          if (WORD_SIZE <= 16)
          {
            short *buff0 = buff;
            printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
          }
          else
          {
            int *buff0 = buff;
            printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
          }
        }
#endif

        errors += check_buffer(buff, get_sampling_freq(i), get_signal_freq(i, k));
      }
    }
  }

  if (errors)
    printf("TEST FAILURE\n");
  else
    printf("TEST SUCCESS\n");

  return errors;
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
