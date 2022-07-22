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
#ifdef USE_FFT
#include "kiss_fft.h"
#endif


#define ERROR_RATE 20
#if WORD_SIZE == 32
#define ELEM_SIZE 4
#elif WORD_SIZE == 16
#define ELEM_SIZE 2
#else
#error Unsupported word size
#endif


#define BUFF_SIZE ((NB_ELEM)*ELEM_SIZE)
#define BLOCK_SIZE (NB_ELEM*ELEM_SIZE)

static uint8_t buff[NB_ITF][2][BUFF_SIZE*NB_CHANNELS];
static uint8_t ch_buff[NB_ITF][NB_CHANNELS][BUFF_SIZE];
static struct pi_device i2s[NB_ITF];
static int max[NB_ITF][NB_CHANNELS];
static int min[NB_ITF][NB_CHANNELS];



#ifdef USE_FFT
static kiss_fft_cpx buff_complex[NB_ELEM];
static kiss_fft_cpx buff_complex_out[NB_ELEM];
static kiss_fft_cfg cfg;
#endif

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

  //printf("    Got bound error rate %f (bound: 0x%x, got: 0x%x)\n", error, bound, value);

  return error > error_margin;
}

#ifndef USE_FFT
static int get_incr_start(int start_value, int itf, int channel, int *error)
{

  if (((start_value - INCR_START0_0) % INCR_VALUE0_0) != 0)
  {
    *error = 1;
    return 0;
   }
  return start_value;
}

static int get_incr_next(uint32_t value, int itf, int channel)
{
    uint64_t incr_value = (uint64_t)value + INCR_VALUE0_0;
    uint64_t incr_value_trunc = incr_value & ((1ULL<<WORD_SIZE) - 1);

    if (incr_value_trunc != incr_value)
    {
      value = INCR_START0_0;
      return value;
    }

    return incr_value;
}
#endif

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

  return SIGNAL_FREQ_0_0;
}

static int check_buffer(uint8_t *buff, int sampling_freq, int signal_freq)
{
#ifdef USE_FFT
  for (int i=0; i<NB_ELEM; i++)
  {
    //printf("%x\n", (((int16_t *)buff)[i+16]));

#if WORD_SIZE <= 16
    buff_complex[i].r = (float)(((int16_t *)buff)[i]);
#else
    buff_complex[i].r = (float)(((int32_t *)buff)[i+16]);
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

 // printf("    Got error rate %d %% (expected: %d, got %d)\n", (int)error, (int)signal_freq, (int)freq);

  return error > ERROR_RATE;
#else

  uint16_t *check_buff = (uint16_t *)buff;
  int error = 0;
  uint16_t expected = get_incr_start(check_buff[0], 0, 0, &error);  
  if (error)
  {
    printf("Could not find initial value from buffer\n");
    return -1;
  }

  for (int i=0; i<NB_ELEM; i++)
  {
    //printf("%x %x\n", check_buff[i], expected);

    if (expected != check_buff[i])
    {
      printf("Error at index %d, expected %x, got %x\n", i, expected, check_buff[i]);
      return -1;
    }

    expected = get_incr_next(expected, 0, 0);
  }

  return 0;

#endif
}

static int test_entry()
{
  int errors = 0;

  printf("NB_ELEM %d\n", NB_ELEM);

  pi_bsp_init();

#if defined(__GAP8__) || (defined(PULP_CHIP_FAMILY) && (PULP_CHIP_FAMILY == CHIP_GAP8))
  pi_pad_set_function(PI_PAD_37_B14_I2S1_SDI, PI_PAD_37_B14_I2S1_SDI_FUNC0);
  pi_pad_set_function(PI_PAD_36_A15_I2S1_WS, PI_PAD_36_A15_I2S1_WS_FUNC0);
  pi_pad_set_function(PI_PAD_35_B13_I2S1_SCK, PI_PAD_35_B13_I2S1_SCK_FUNC0);
  pi_pad_set_function(PI_PAD_55_A24_I2S0_SCK, PI_PAD_55_A24_I2S0_SCK_FUNC0);
  pi_pad_set_function(PI_PAD_56_A26_I2S0_WS, PI_PAD_56_A26_I2S0_WS_FUNC0);
  pi_pad_set_function(PI_PAD_57_B23_I2S0_SDI, PI_PAD_57_B23_I2S0_SDI_FUNC0);
#endif

  for (int i=0; i<NB_ITF; i++)
  {
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.ws_type = 0;
    i2s_conf.pingpong_buffers[0] = buff[i][0];
    i2s_conf.pingpong_buffers[1] = buff[i][1];
    i2s_conf.block_size = BLOCK_SIZE;
    i2s_conf.frame_clk_freq = get_sampling_freq(i);
    i2s_conf.itf = i;
#if defined(PDM) && PDM == 1
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.pdm_decimation = 64;
#else
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S | PI_I2S_CH_FMT_DATA_ORDER_MSB;
#endif
    i2s_conf.word_size = WORD_SIZE;
    i2s_conf.channels = NB_CHANNELS;

    pi_open_from_conf(&i2s[i], &i2s_conf);

    if (pi_i2s_open(&i2s[i]))
      return -1;
  }

  for (int i=0; i<NB_ITF; i++)
  {
    if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
      return -1;
  }

  void *chunk[NB_ITF];
  for (int i=0; i<NB_ITF; i++)
  {
    unsigned int size;

    pi_i2s_read(&i2s[i], &chunk[i], &size);
  }

  for (int i=0; i<NB_ITF; i++)
  {
    pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s[i]);
  }

#if 0
  printf("Raw buffer size\n");
  for (int j=0; j<BLOCK_SIZE/ELEM_SIZE; j++)
  {
    if (WORD_SIZE <= 16)
    {
      short *buff0 = buff[1][0];
      printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
    }
    else
    {
      int *buff0 = buff[1][0];
      printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
    }
  }
#endif

#ifdef USE_FFT
  cfg = kiss_fft_alloc(NB_ELEM, 0, NULL, NULL);
#endif

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {
      min[i][k] = 0;
      max[i][k] = 0;

      for (int j=0; j<NB_ELEM; j++)
      {
        int elem_value = 0;

        for (int l=0; l<ELEM_SIZE; l++)
        {
          int value = buff[i][0][(j*NB_CHANNELS + k)*ELEM_SIZE+l];
          ch_buff[i][k][j*ELEM_SIZE+l] = value;

          elem_value |= value << (l*8);
        }

        if (WORD_SIZE == 16)
        {
          elem_value = (elem_value << 16) >> 16;
        }

        //printf("VAL %x %d\n", (unsigned short)elem_value, elem_value);

        if (elem_value > max[i][k])
          max[i][k] = elem_value;
        if (elem_value < min[i][k])
          min[i][k] = elem_value;
      }
    }
  }

#if 0
  printf("Sorted buffer\n");
  for (int j=0; j<BUFF_SIZE/2; j++)
  {
    short *buff0 = ch_buff[0][0];
    printf("%d\n", buff0[j]);
  }
#endif

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {

      if (check_error((1ULL<<(WORD_SIZE-1)) - 1, max[i][k], 0.2))
        return -1;

      if (check_error(-(1LL<<(WORD_SIZE-1)), min[i][k], 0.2))
        return -1;

      errors += check_buffer(ch_buff[i][k], get_sampling_freq(i), get_signal_freq(i, k));
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
