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

#include "rt/rt_api.h"
#include "kiss_fft.h"

#define USE_1CH      0
#define USE_2CH      1


#if DUAL == 1
#if defined(MODE) && MODE == USE_2CH
#define NB_BUFFER 4
#else
#define NB_BUFFER 2
#endif
#elif defined(MODE) && MODE == USE_2CH
#define NB_BUFFER 2
#else
#define NB_BUFFER 1
#endif

#define ERROR_RATE 20
#define ELEM_SIZE 2
#define NB_ELEM 256
#define BUFF_SIZE ((NB_ELEM+16)*ELEM_SIZE)
#define DUAL_BUFF_SIZE ((NB_ELEM+16)*ELEM_SIZE*(NB_BUFFER))

static uint8_t buff[NB_ITF*NB_BUFFER][DUAL_BUFF_SIZE];
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

static int get_signal_freq(int itf, int channel)
{
  if (itf == 0)
  {
#if defined(MODE) && MODE == USE_2CH
    if (channel == 0)
      return SIGNAL_FREQ_0_0;
    else if (channel == 2)
      return SIGNAL_FREQ_0_1;
    else if (channel == 1)
      return SIGNAL_FREQ_1_0;
    else
      return SIGNAL_FREQ_1_1;
#else
    if (channel == 0)
      return SIGNAL_FREQ_0_0;
    else if (channel == 1)
      return SIGNAL_FREQ_0_1;
#endif
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

    buff_complex[i].r = (float)(((int16_t *)buff)[i+16]);
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

  printf("Got error rate %d %% (expected: %d, got %d)\n", (int)error, (int)signal_freq, (int)freq);

  return error > ERROR_RATE;
}

int main()
{
  rt_i2s_t *i2s[NB_ITF];
  rt_event_t *event[NB_ITF];
  int errors = 0;

  printf("Entering main controller\n");

  cfg = kiss_fft_alloc(NB_ELEM, 0, NULL, NULL);

  if (rt_event_alloc(NULL, 1)) return -1;

  for (int i=0; i<NB_ITF; i++)
  {
    rt_i2s_conf_t i2s_conf;
    rt_i2s_conf_init(&i2s_conf);

    i2s_conf.frequency = get_sampling_freq(i);
    i2s_conf.dual = DUAL;
    i2s_conf.pdm = PDM;
    i2s_conf.width = 16;
    i2s_conf.id = i;

#if defined(MODE) && MODE == USE_2CH
    if (DUAL == 1)
      i2s_conf.flags = RT_I2S_FLAGS_MUX_2CH_DUAL;
    else
      i2s_conf.flags = RT_I2S_FLAGS_MUX_2CH;
#endif

    i2s[i] = rt_i2s_open(NULL, &i2s_conf, NULL);
    if (i2s == NULL) return -1;
  }

  for (int i=0; i<NB_ITF; i++)
  {
    event[i] = rt_event_get_blocking(NULL);
    rt_i2s_capture(i2s[i], buff[i*NB_BUFFER], DUAL_BUFF_SIZE, event[i]);
  }

  for (int i=0; i<NB_ITF; i++)
  {
    rt_i2s_start(i2s[i]);
  }

  for (int i=0; i<NB_ITF; i++)
  {
    rt_event_wait(event[i]);
    rt_i2s_stop(i2s[i]);
    rt_i2s_close(i2s[i], NULL);
  }

#if 0
  for (int j=0; j<BUFF_SIZE/2; j++)
  {
    short *buff0 = buff[0];
    printf("%d\n", buff0[j]);
  }
#endif

  if (DUAL == 1 || MODE == USE_2CH)
  {
    for (int i=0; i<NB_ITF; i++)
    {
      short *temp_buff[NB_BUFFER];

      for (int k=0; k<NB_BUFFER; k++)
      {
        temp_buff[k] = buff[i*NB_BUFFER+k];
      }

      for (int j=0; j<BUFF_SIZE; j++)
      {
        for (int k=NB_BUFFER-1; k>=0; k--)
        {
          temp_buff[k][j] = temp_buff[0][j*NB_BUFFER + k];
        }
      }
    }
  }

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_BUFFER; k++)
    {
      errors += check_buffer(buff[i*NB_BUFFER+k], get_sampling_freq(i), get_signal_freq(i, k));
    }
  }

  if (errors)
    printf("TEST FAILURE\n");
  else
    printf("TEST SUCCESS\n");

  return errors;
}
