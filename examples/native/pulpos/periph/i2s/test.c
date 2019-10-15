/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

/*
 * Copyright (c) 2017 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rt/rt_api.h"
#include "stdio.h"
#include "wav_synchro.h"

recordBuf buff;
static int enqueued = 0;
static int finished = 0;
static rt_i2s_t *i2s;

extern RT_L2_DATA I2sDescriptor I2sOutHeader;

// This is called anytime a buffer is full
// Just reenqueue a new one reusing the same buffer
static void buffer_ready(void *arg)
{
  int id = ((int)arg)+2;//increase subbuffer index given as argument
  finished++;
  enqueued++;
  if (id < NB_BUF)
    rt_i2s_capture(i2s, buff.buff[id], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)id));
}

int main()
{
  // Allocate events to schedule the end-of-transfer events
  if (rt_event_alloc(NULL, 8)) return -1;

  // Initialize desired configuration
  rt_i2s_conf_t i2s_conf;
  rt_i2s_conf_init(&i2s_conf);

  i2s_conf.frequency = 16000;
  i2s_conf.dual = 0;
  i2s_conf.width = 16;
  i2s_conf.pdm = 1;
  i2s_conf.decimation_log2 = 6;

  // Open the microphone
  i2s = rt_i2s_open("microphone", &i2s_conf, NULL);
  if (i2s == NULL) return -1;

  //init buffer
  for(unsigned cntJ=0; cntJ<(NB_BUF);cntJ++) {
    for(unsigned cnt=0; cnt<(NB_SUBUF_SAMPLES);cnt++) {
      buff.buff[cntJ][cnt] = 0xaaaa;
    }
  }
  //init struct to synchronize with python
  I2sOutHeader.l2Addr = (unsigned int) buff.header;
  I2sOutHeader.size = BUFF_SIZE;

  //syncrhonyze with python
  printf("c: Ready to record audio from microphones\n");
  I2sOutHeader.cStarted = 1;
	while (((volatile I2sDescriptor *) &I2sOutHeader)->micReady == 0) {};

  // Now always make sure 2 buffers are always ready to not lose any data
  // The end-of-transfer event will reenqueue a new transfer when one finishes
  enqueued += 2;
  rt_i2s_capture(i2s, buff.buff[0], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)0));
  rt_i2s_capture(i2s, buff.buff[1], NB_SUBUF_SAMPLES*2, rt_event_get(NULL, buffer_ready, (void *)1));

  // Start acquisition
  rt_i2s_start(i2s);

  // Loop until we processed enough buffers
  while(finished < NB_BUF) {
    rt_event_execute(NULL, 1);
  }

  // Stop and close the microphone
  rt_i2s_close(i2s, NULL);

  CreateWAVHeader((unsigned char *) buff.header, &I2sOutHeader);

  //synchronize with python before to exit as it may download the wav
  I2sOutHeader.recReady = 1;
  while(((volatile I2sDescriptor *) &I2sOutHeader)->recReady == 1);

  return 0;
}
